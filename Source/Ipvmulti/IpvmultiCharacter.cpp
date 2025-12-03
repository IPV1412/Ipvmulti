// Copyright Epic Games, Inc. All Rights Reserved.


#include "IpvmultiCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Actors/Proyectil.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);


//////////////////////////////////////////////////////////////////////////
// AIpvmultiCharacter


AIpvmultiCharacter::AIpvmultiCharacter():
CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this,&ThisClass::OnCreateSessionComplete)),
FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this,&ThisClass::OnFindSessionComplete)),
JoinSessionsCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this,&ThisClass::OnJoinSessionComplete))
{
   // Set size for collision capsule
   GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
     
   // Don't rotate when the controller rotates. Let that just affect the camera.
   bUseControllerRotationPitch = false;
   bUseControllerRotationYaw = false;
   bUseControllerRotationRoll = false;


   // Configure character movement
   GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input... 
   GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate


   // Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
   // instead of recompiling to adjust them
   GetCharacterMovement()->JumpZVelocity = 700.f;
   GetCharacterMovement()->AirControl = 0.35f;
   GetCharacterMovement()->MaxWalkSpeed = 500.f;
   GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
   GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
   GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;


   // Create a camera boom (pulls in towards the player if there is a collision)
   CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
   CameraBoom->SetupAttachment(RootComponent);
   CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character 
   CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller


   // Create a follow camera
   FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
   FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
   FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


   // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
   // are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)


   IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
   if (OnlineSubsystem)
   {
      OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
      if (GEngine)
      {
         GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Purple,
            FString::Printf(TEXT("Found Online Subsystem %s"), * OnlineSubsystem->GetSubsystemName().ToString()));
        
      }
   }
  
   //Initialize the player's Health
   MaxHealth = 100.0f;
   CurrentHealth = MaxHealth;


   //Initialize projectile class
   ProjectileClass = AProyectil::StaticClass();
  
   //Initialize fire rate
   FireRate = 0.25f;
   bIsFiringWeapon = false;


   //Inicializar Munición
   MaxAmmo     = 5;
   CurrentAmmo = MaxAmmo;


   // Habilitar replicación en el actor
   //bReplicates = true;
}


void AIpvmultiCharacter::BeginPlay()
{
   Super::BeginPlay();


   if (HasAuthority())
   {
      // Asegurar que el servidor comience con munición = MaxAmmo
      CurrentAmmo = MaxAmmo;
   }
}


void AIpvmultiCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps(OutLifetimeProps);


   //Replicate current health.
   DOREPLIFETIME(AIpvmultiCharacter, CurrentHealth);
  
   // Replica CurrentAmmo
   DOREPLIFETIME(AIpvmultiCharacter, CurrentAmmo);
}


//////////////////////////////////////////////////////////////////////////
// Input


void AIpvmultiCharacter::OnRep_CurrentHealth()
{
   OnHealthUpdate();
}


void AIpvmultiCharacter::OnRep_CurrentAmmo()
{
   if (IsLocallyControlled())
   {
      FString ammoMsg = FString::Printf(TEXT("Munición actual: %d"), CurrentAmmo);
      GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, ammoMsg);
   }
}


void AIpvmultiCharacter::NotifyControllerChanged()
{
   Super::NotifyControllerChanged();


   // Add Input Mapping Context
   if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
   {
      if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
      {
         Subsystem->AddMappingContext(DefaultMappingContext, 0);
      }
   }
}


void AIpvmultiCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
   // Set up action bindings
   if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
     
      // Jumping
      EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
      EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);


      // Moving
      EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AIpvmultiCharacter::Move);


      // Looking
      EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AIpvmultiCharacter::Look);


      // Fire
      EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AIpvmultiCharacter::StartFire);


   }
   else
   {
      UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
   }
  
   // Handle firing projectiles
   //PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AIpvmultiCharacter::StartFire);


}


void AIpvmultiCharacter::OnHealthUpdate_Implementation()
{//Client-specific functionality
   if (IsLocallyControlled())
   {
      FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), CurrentHealth);
      GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);


      if (CurrentHealth <= 0)
      {
         FString deathMessage = FString::Printf(TEXT("You have been killed."));
         GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
      }
   }
  
   if (CurrentHealth <= 0)
   {
      HandleDeath();
   }
  
   //Server-specific functionality
   if (GetLocalRole() == ROLE_Authority)
   {
      FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), CurrentHealth);
      GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
   }
}


void AIpvmultiCharacter::SetCurrentHealth(float healthValue)
{
   if (GetLocalRole() == ROLE_Authority)
   {
      CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
      OnHealthUpdate();
   }
}


float AIpvmultiCharacter::TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent,
   AController* EventInstigator, AActor* DamageCauser)
{
   float damageApplied = CurrentHealth - DamageTaken;
   SetCurrentHealth(damageApplied);
   return damageApplied;
}


void AIpvmultiCharacter::OpenLobby()
{
   UWorld* World = GetWorld();
   if (!World) return;
   World->ServerTravel("Game/ThirdPerson/Maps/ThirdPersonMap.ThirdPersonMap?listen");
}


void AIpvmultiCharacter::CallOpenLevel(const FString& IPAndres)
{
   UGameplayStatics::OpenLevel(this,*IPAndres);
}


void AIpvmultiCharacter::Move(const FInputActionValue& Value)
{
   // input is a Vector2D
   FVector2D MovementVector = Value.Get<FVector2D>();


   if (Controller != nullptr)
   {
      // find out which way is forward
      const FRotator Rotation = Controller->GetControlRotation();
      const FRotator YawRotation(0, Rotation.Yaw, 0);


      // get forward vector
      const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
  
      // get right vector
      const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);


      // add movement
      AddMovementInput(ForwardDirection, MovementVector.Y);
      AddMovementInput(RightDirection, MovementVector.X);
   }
}


void AIpvmultiCharacter::Look(const FInputActionValue& Value)
{
   // input is a Vector2D
   FVector2D LookAxisVector = Value.Get<FVector2D>();


   if (Controller != nullptr)
   {
      // add yaw and pitch input to controller
      AddControllerYawInput(LookAxisVector.X);
      AddControllerPitchInput(LookAxisVector.Y);
   }
}


void AIpvmultiCharacter::StartFire()
{
   if (!bIsFiringWeapon)
   {
      bIsFiringWeapon = true;
      UWorld* World = GetWorld();
      World->GetTimerManager().SetTimer(FiringTimer, this, &AIpvmultiCharacter::StopFire, FireRate, false);
      //HandleFire();
      TryFire();
   }
}


void AIpvmultiCharacter::StopFire()
{
   bIsFiringWeapon = false;
}


void AIpvmultiCharacter::HandleDeath_Implementation()
{
   // Activar Ragdoll
   GetMesh()->SetSimulatePhysics(true);
   GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
   GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);


   // Mostrar UI SOLO al jugador local
   if (IsLocallyControlled())
   {
      // Llama al evento ShowGO del Blueprint
      FName FunctionName = FName("ShowGO");
      if (UFunction* Func = FindFunction(FunctionName))
      {
         FString deathMessage = FString::Printf(TEXT("GAMEOVER."));
         GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
         ProcessEvent(Func, nullptr);
      }
   }


   // Desactivar input SOLO para el jugador que controla este pawn
   if (APlayerController* PC = Cast<APlayerController>(GetController()))
   {
      DisableInput(PC);
     
      FTimerHandle DeathTimer;
      GetWorld()->GetTimerManager().SetTimer(DeathTimer, [this, PC]()
      {
         if (HasAuthority())
         {
            if (AGameModeBase* GM = UGameplayStatics::GetGameMode(this))
            {
               this->Destroy();
               GM->RestartPlayer(PC);
            }
         }
      },  3.0f, false);
   }
}


void AIpvmultiCharacter::TryFire()
{
   // Solo dispara si hay munición
   if (CurrentAmmo > 0)
   {
      if (HasAuthority())
      {
         // Ya estamos en servidor: spawn + decrementar munición
         HandleFire();
         SetCurrentAmmo(CurrentAmmo - 1);
      }
      else
      {
         // Cliente envía petición al servidor
         ServerHandleFire();
      }
   }
   else
   {
      if (IsLocallyControlled())
      {
         GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Sin munición."));
      }
   }
}




bool AIpvmultiCharacter::ServerHandleFire_Validate()
{
   return true;
}


void AIpvmultiCharacter::ServerHandleFire_Implementation()
{
   // En servidor, verificar munición y disparar
   if (CurrentAmmo <= 0)
   {
      return;
   }
   HandleFire();
   SetCurrentAmmo(CurrentAmmo - 1);
}


void AIpvmultiCharacter::HandleFire_Implementation()
{
   FVector spawnLocation = GetActorLocation() + ( GetActorRotation().Vector()  * 100.0f ) + (GetActorUpVector() * 50.0f);
   FRotator spawnRotation = GetActorRotation();


   FActorSpawnParameters spawnParameters;
   spawnParameters.Instigator = GetInstigator();
   spawnParameters.Owner = this;


   AProyectil* spawnedProjectile = GetWorld()->SpawnActor<AProyectil>(spawnLocation, spawnRotation, spawnParameters);
}


void AIpvmultiCharacter::RestoreAmmo()
{
   if (HasAuthority())
   {
      SetCurrentAmmo(MaxAmmo);
   }
   else
   {
      ServerRestoreAmmo();
   }
}


bool AIpvmultiCharacter::ServerRestoreAmmo_Validate()
{
   return true;
}


void AIpvmultiCharacter::ServerRestoreAmmo_Implementation()
{
   SetCurrentAmmo(MaxAmmo);
}


void AIpvmultiCharacter::SetCurrentAmmo(int32 NewAmmo)
{
   if (!HasAuthority()) return;


   CurrentAmmo = FMath::Clamp(NewAmmo, 0, MaxAmmo);
}


//-------------------------------//
void AIpvmultiCharacter::OnFindSessionComplete(bool bWasSuccessful)
{
   for (auto Result:SessionSearch->SearchResults)
   {
      FString ID = Result.GetSessionIdStr();
      FString User = Result.Session.OwningUserName;
      FString MatchType;
      Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);
     
      if(GEngine)
      {
         GEngine->AddOnScreenDebugMessage(
            -1,
            15.f,
            FColor::Orange,
            FString::Printf(TEXT("Id: %s, User: %s"), *ID, *User)
         );
      }
      else
      {
         GEngine->AddOnScreenDebugMessage(
            -1,
            15.f,
            FColor::Red,
            FString(TEXT("Find Session Failed"))
         );
      }
     
      if (MatchType == FString("FreeForAll"))
      {
         if(GEngine)
         {
            GEngine->AddOnScreenDebugMessage(
               -1,
               15.f,
               FColor::Black,
               FString::Printf(TEXT("Joining Match Type:%s"),*MatchType)
            );
           
            //Delegate-list
            OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionsCompleteDelegate);


            Result.Session.SessionSettings.bUseLobbiesIfAvailable = true;
            Result.Session.SessionSettings.bUsesPresence = true;


            const ULocalPlayer * LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
            OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result);
         }
           
      }
   }
}




void AIpvmultiCharacter::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
   if (!OnlineSessionInterface.IsValid()) return;


   FString Address;


   if (OnlineSessionInterface->GetResolvedConnectString(SessionName, Address))
   {
      if(GEngine)
      {
         GEngine->AddOnScreenDebugMessage(-1,15.f,FColor::Blue,FString::Printf(TEXT("Connect string: %s"), *Address));
      }
      APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();


      if (PlayerController)
      {
         PlayerController->ClientTravel(Address, TRAVEL_Absolute);
      }
   }
}




void AIpvmultiCharacter::JoinGameSession()
{
   if (!OnlineSessionInterface.IsValid()) return;
   //Delegate-list
   OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);
   //FindSession
   SessionSearch = MakeShareable(new FOnlineSessionSearch());
   SessionSearch->MaxSearchResults = 10000;
   SessionSearch->bIsLanQuery = false;
   SessionSearch->QuerySettings.Set(SEARCH_PRESENCE,true,EOnlineComparisonOp::Equals);


   const ULocalPlayer * LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
   OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(),SessionSearch.ToSharedRef());
  
}


void AIpvmultiCharacter::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
   if(bWasSuccessful)
   {
      if (GEngine)
      {
         GEngine->AddOnScreenDebugMessage(
            -1,
            15.f,
            FColor::Blue,
            FString::Printf(TEXT("Created Session %s"), *SessionName.ToString())
         );
      }
      UWorld* World = GetWorld();
      if (World)
      {
         World->ServerTravel("/Game/IPV-Multi/Mapas/IPV_Level?listen"); 
      }
   }
   else
   {
      GEngine->AddOnScreenDebugMessage(
         -1,
         15.f,
         FColor::Red,
         FString(TEXT("Create Session Failed"))
      );
   }


}


void AIpvmultiCharacter::CreateGameSession()
{
   if (!OnlineSessionInterface.IsValid()) return;
   FNamedOnlineSession* ExistingSession = OnlineSessionInterface -> GetNamedSession(NAME_GameSession);
   if (ExistingSession)
   {
      OnlineSessionInterface->DestroySession(NAME_GameSession);
   }
   //Delegate-list
   OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);
   //CreateSession
   TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
   SessionSettings -> bIsLANMatch = false;
   SessionSettings -> NumPublicConnections = 4;
   SessionSettings -> bAllowJoinInProgress = true;
   SessionSettings -> bAllowJoinViaPresence = true;
   SessionSettings -> bShouldAdvertise = true;
   SessionSettings -> bUsesPresence = true;
   SessionSettings -> bUseLobbiesIfAvailable = true;
   SessionSettings -> Set(FName("MatchType"),FString("FreeForAll"),EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);


   const ULocalPlayer * LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
   OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(),NAME_GameSession,*SessionSettings);
}
