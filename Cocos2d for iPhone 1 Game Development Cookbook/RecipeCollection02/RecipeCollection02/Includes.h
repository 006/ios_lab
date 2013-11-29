#import "Ch4_BasicSetup.h"
#import "Ch4_CollisionResponse.h"
#import "Ch4_DifferentShapes.h"
#import "Ch4_DraggingAndFiltering.h"
#import "Ch4_PhysicalProperties.h"
#import "Ch4_Impulses.h"
#import "Ch4_Forces.h"
#import "Ch4_AsyncBodyDestruction.h"
#import "Ch4_Joints.h"
#import "Ch4_Vehicles.h"
#import "Ch4_CharacterMovement.h"
#import "Ch4_Bullets.h"
#import "Ch4_Rope.h"
#import "Ch4_TopDownIsometric.h"
#import "Ch5_SwitchingScenes.h"
#import "Ch5_SceneTransitions.h"
#import "Ch5_UsingCCLayerMultiplex.h"
#import "Ch5_UsingCCLabel.h"
#import "Ch5_UsingCCMenu.h"
#import "Ch5_ShadowedLabels.h"
#import "Ch5_UIKitAlerts.h"
#import "Ch5_WrappingUIKit.h"
#import "Ch5_MenuWindows.h"
#import "Ch5_HorizScrollMenu.h"
#import "Ch5_VertSlidingMenuGrid.h"
#import "Ch5_LoadingScreen.h"
#import "Ch5_Minimap.h"
#import "Ch6_SoundsAndMusic.h"
#import "Ch6_AudioProperties.h"
#import "Ch6_FadingSoundsAndMusic.h"
#import "Ch6_AudioInGame.h"
#import "Ch6_PositionalAudio.h"
#import "Ch6_MeteringMusic.h"
#import "Ch6_MeteringDialogue.h"
#import "Ch6_StreamingAudio.h"
#import "Ch6_RecordingAudio.h"
#import "Ch6_iPodLibrary.h"
#import "Ch6_MIDISynthesization.h"

static NSString *recipeNames[] = {
	@"Ch4_BasicSetup",
	@"Ch4_CollisionResponse",
	@"Ch4_DifferentShapes",
	@"Ch4_DraggingAndFiltering",
	@"Ch4_PhysicalProperties",
	@"Ch4_Impulses",
	@"Ch4_Forces",
	@"Ch4_AsyncBodyDestruction",
	@"Ch4_Joints",
	@"Ch4_Vehicles",
	@"Ch4_CharacterMovement",
	@"Ch4_Bullets",
	@"Ch4_Rope",
	@"Ch4_TopDownIsometric",
    @"Ch5_SwitchingScenes",
    @"Ch5_SceneTransitions",
	@"Ch5_UsingCCLayerMultiplex",
	@"Ch5_UsingCCLabel",
    @"Ch5_UsingCCMenu",
    @"Ch5_ShadowedLabels",
	@"Ch5_UIKitAlerts",
    @"Ch5_WrappingUIKit",
    @"Ch5_MenuWindows",
	@"Ch5_HorizScrollMenu",
	@"Ch5_VertSlidingMenuGrid",
    @"Ch5_LoadingScreen",
    @"Ch5_Minimap",
	@"Ch6_SoundsAndMusic",
	@"Ch6_AudioProperties",
	@"Ch6_FadingSoundsAndMusic",
	@"Ch6_AudioInGame",
	@"Ch6_PositionalAudio",
	@"Ch6_MeteringMusic",
	@"Ch6_MeteringDialogue",
	@"Ch6_StreamingAudio",
	@"Ch6_RecordingAudio",
	@"Ch6_iPodLibrary",
	@"Ch6_MIDISynthesization"
};