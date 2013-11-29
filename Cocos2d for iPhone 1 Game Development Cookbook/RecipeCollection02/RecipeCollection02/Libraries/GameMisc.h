#import "GameObject.h"

@interface GameMisc : GameObject {
	@public
		float life;
}
@property (readonly) int type;
@property (readwrite, assign) float life;

@end