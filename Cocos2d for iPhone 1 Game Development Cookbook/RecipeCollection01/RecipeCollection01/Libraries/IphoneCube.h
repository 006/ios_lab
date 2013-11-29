#import "Vector3D.h"
#import "Cube3D.h"

@interface IphoneCube : Cube3D
{
	CCTexture2D *front;
	CCTexture2D *back;
	CCTexture2D *side;
}
-(id) init;
-(void) draw;

@end

@implementation IphoneCube

-(id) init {
    self = [super init];
    if (self) {
		front = [[CCTexture2D alloc] initWithImage:[UIImage imageNamed:@"iphone4_front.jpg"]];
		back = [[CCTexture2D alloc] initWithImage:[UIImage imageNamed:@"iphone4_back.jpg"]];
		side = [[CCTexture2D alloc] initWithImage:[UIImage imageNamed:@"iphone4_side.jpg"]];
    }
    return self;
}

-(void) draw {
	//Vertices for each side of the cube
	float width = 0.5f;
	float height = 1.0f;
	float depth = 0.1f;
	
	const GLfloat frontVertices[] = {
		-width, -height,  depth,
        width, -height,  depth,
        -width,  height,  depth,
        width,  height,  depth,
	};
	const GLfloat backVertices[] = {
		width, -height, -depth,
        -width,  -height, -depth,
        width, height, -depth,
        -width,  height, -depth,
	};
	const GLfloat leftVertices[] = {
		-width, -height,  depth,
        -width,  height,  depth,
        -width, -height, -depth,
        -width,  height, -depth,
	};
	const GLfloat rightVertices[] = {
		width, -height, -depth,
        width,  height, -depth,
        width, -height,  depth,
        width,  height,  depth,
	};
	const GLfloat topVertices[] = {
		-width,  height,  depth,
        width,  height,  depth,
        -width,  height, -depth,
        width,  height, -depth,
	};
	const GLfloat bottomVertices[] = {
		-width, -height,  depth,
        -width, -height, -depth,
        width, -height,  depth,
        width, -height, -depth,
	};
	
	//Coordinates for our texture to map it to a cube side
	const GLfloat textureCoordinates[] = {
		0, 0,
        1, 0,
        0, 1,
        1, 1,
	};
	
	//We enable back face culling to properly set the depth buffer
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//We are not using GL_COLOR_ARRAY
	glDisableClientState(GL_COLOR_ARRAY);
	
	//Replace the current matrix with the identity matrix 
	glLoadIdentity();

	//Translate and rotate
	glTranslatef(translation3D.x, translation3D.y, translation3D.z);
	//glRotatef(rotation3DAngle, rotation3DAxis.x, rotation3DAxis.y, rotation3DAxis.z);
	
	glRotatef(rotation3DAxis.x,1,0,0);
	glRotatef(rotation3DAxis.y,0,1,0);
	glRotatef(rotation3DAxis.z,0,0,1);
	
	//Here we define our vertices, set our textures or colors and finally draw the cube sides
    
	glBindTexture(GL_TEXTURE_2D, front.name);
	glVertexPointer(3, GL_FLOAT, 0, frontVertices);
	glTexCoordPointer(2, GL_FLOAT, 0,  textureCoordinates);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glBindTexture(GL_TEXTURE_2D, back.name);
    glVertexPointer(3, GL_FLOAT, 0, backVertices);
	glTexCoordPointer(2, GL_FLOAT, 0,  textureCoordinates);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glVertexPointer(3, GL_FLOAT, 0, leftVertices);
	glTexCoordPointer(2, GL_FLOAT, 0,  textureCoordinates);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
    glVertexPointer(3, GL_FLOAT, 0, rightVertices);
	glTexCoordPointer(2, GL_FLOAT, 0,  textureCoordinates);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
    glVertexPointer(3, GL_FLOAT, 0, topVertices);
	glTexCoordPointer(2, GL_FLOAT, 0,  textureCoordinates);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
    glVertexPointer(3, GL_FLOAT, 0, bottomVertices);
	glTexCoordPointer(2, GL_FLOAT, 0,  textureCoordinates);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	//We re-enable the default render state
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_CULL_FACE);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

}
@end