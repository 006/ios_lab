#import "GameObject.h"
#import "Vector3D.h"

@interface GameIsoObject : GameObject {
	@public
		float yModifier;	//This is typically half the height of the object. It allows us to change the sprite y.
		float actualImageSize; //This is the actual size of the image (48x48, 96x96, etc)
		float inGameSize;	//This is how large the object in the game is.
		float zModifier;	//Changes the depth testing for this object.
		CCSprite *spriteShadow;
		Vector3D *bounceCoefficient;	//x, y, z, lower is bouncier for Z
		Vector3D *rollCoefficient;
}
@property (readonly) int type;
@property (readwrite, assign) float yModifier;
@property (readwrite, assign) float actualImageSize;
@property (readwrite, assign) float inGameSize;
@property (readwrite, assign) float zModifier;
@property (readwrite, assign) Vector3D *bounceCoefficient;
@property (readwrite, assign) Vector3D *rollCoefficient;
@property (nonatomic, retain) CCSprite *spriteShadow;

@end