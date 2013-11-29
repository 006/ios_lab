//TO DO -	Put all the GKSessionDelegate code in the actual recipe file. It will actually be the "session manager".
//			The GKVoiceChatClient code can be its own thing called VoiceChatManager. This can be modular.
//			Basically we're going to clean up and lean-up a bunch of this stuff.

// Fuck it. Just put all the stuff in the recipe file.

#import <UIKit/UIKit.h>
#import <GameKit/GameKit.h> 

typedef enum {
    PacketTypeVoice = 0,
    PacketTypeStart = 1,
    PacketTypeBounce = 2,
    PacketTypeScore = 3,
    PacketTypeTalking = 4,
    PacketTypeEndTalking = 5
} PacketType;

typedef enum {
    ConnectionStateDisconnected,
    ConnectionStateConnecting,
    ConnectionStateConnected
} ConnectionState;

@interface SessionManager : NSObject <GKSessionDelegate> {
	NSString *sessionID;
	GKSession *myGKSession;
	NSString *currentConfPeerID;
	NSMutableArray *peerList;
	id lobbyDelegate;
	id gameDelegate;
    ConnectionState sessionState;
}

@property (nonatomic, readonly) NSString *currentConfPeerID;
@property (nonatomic, readonly) NSMutableArray *peerList;
@property (nonatomic, assign) id lobbyDelegate;
@property (nonatomic, assign) id gameDelegate;

- (void) setupSession;
- (void) connect:(NSString *)peerID;
- (BOOL) didAcceptInvitation;
- (void) didDeclineInvitation;
- (void) sendPacket:(NSData*)data ofType:(PacketType)type;
- (void) disconnectCurrentCall;
- (NSString *) displayNameForPeer:(NSString *)peerID;

@end

// Class extension for private methods.
@interface SessionManager ()

- (BOOL) comparePeerID:(NSString*)peerID;
- (BOOL) isReadyToStart;
- (void) voiceChatDidStart;
- (void) destroySession;
- (void) willTerminate:(NSNotification *)notification;
- (void) willResume:(NSNotification *)notification;

@end

@interface SessionManager (VoiceManager) <GKVoiceChatClient>

- (void) setupVoice;

@end

@protocol SessionManagerLobbyDelegate

- (void) peerListDidChange:(SessionManager *)session;
- (void) didReceiveInvitation:(SessionManager *)session fromPeer:(NSString *)participantID;
- (void) invitationDidFail:(SessionManager *)session fromPeer:(NSString *)participantID;
- (void) peerConnected:(NSString*)peerID;

@end

@protocol SessionManagerGameDelegate

- (void) voiceChatWillStart:(SessionManager *)session;
- (void) session:(SessionManager *)session didConnectAsInitiator:(BOOL)shouldStart;
- (void) willDisconnect:(SessionManager *)session;
- (void) session:(SessionManager *)session didReceivePacket:(NSData*)data ofType:(PacketType)packetType;

@end

