//Test3D

#import "Vector3D.h"

@interface Cube3D : CCSprite
{
	Vector3D *translation3D;
	Vector3D *rotation3DAxis;
	GLfloat rotation3DAngle;
	bool drawTextured;
}
@property (readwrite, assign) Vector3D *translation3D;
@property (readwrite, assign) Vector3D *rotation3DAxis;
@property (readwrite, assign) GLfloat rotation3DAngle;
@property (readwrite, assign) bool drawTextured;
-(void) draw;
@end

@implementation Cube3D
@synthesize translation3D,rotation3DAxis,rotation3DAngle,drawTextured;

-(void) draw {
	//Vertices for each side of the cube
	const GLfloat frontVertices[] = {
		-0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
	};
	const GLfloat backVertices[] = {
		-0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
	};
	const GLfloat leftVertices[] = {
		-0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
	};
	const GLfloat rightVertices[] = {
		0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
	};
	const GLfloat topVertices[] = {
		-0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
	};
	const GLfloat bottomVertices[] = {
		-0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f, -0.5f, -0.5f,
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
	
	//We disable GL_TEXTURE_COORD_ARRAY if not using a texture
	if(!drawTextured){
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	
	//Replace the current matrix with the identity matrix 
	glLoadIdentity();

	//Translate and rotate
	glTranslatef(translation3D.x, translation3D.y, translation3D.z);
	glRotatef(rotation3DAngle, rotation3DAxis.x, rotation3DAxis.y, rotation3DAxis.z);

	//Bind our texture if neccessary
	if(drawTextured){
		glBindTexture(GL_TEXTURE_2D, texture_.name);
	}
	
	//Here we define our vertices, set our textures or colors and finally draw the cube sides
    glVertexPointer(3, GL_FLOAT, 0, frontVertices);
	if(drawTextured){ glTexCoordPointer(2, GL_FLOAT, 0,  textureCoordinates); }
	else{ glColor4f(1.0f, 0.0f, 0.0f, 1.0f); }
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
    glVertexPointer(3, GL_FLOAT, 0, backVertices);
	if(drawTextured){ glTexCoordPointer(2, GL_FLOAT, 0,  textureCoordinates); }
	else{ glColor4f(1.0f, 1.0f, 0.0f, 1.0f); }
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
    glVertexPointer(3, GL_FLOAT, 0, leftVertices);
	if(drawTextured){ glTexCoordPointer(2, GL_FLOAT, 0,  textureCoordinates); }
	else{ glColor4f(1.0f, 0.0f, 1.0f, 1.0f); }
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
    glVertexPointer(3, GL_FLOAT, 0, rightVertices);
	if(drawTextured){ glTexCoordPointer(2, GL_FLOAT, 0,  textureCoordinates); }
	else{ glColor4f(0.0f, 1.0f, 1.0f, 1.0f); }
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
    glVertexPointer(3, GL_FLOAT, 0, topVertices);
	if(drawTextured){ glTexCoordPointer(2, GL_FLOAT, 0,  textureCoordinates); }
	else{ glColor4f(0.0f, 1.0f, 0.0f, 1.0f); }
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
    glVertexPointer(3, GL_FLOAT, 0, bottomVertices);
	if(drawTextured){ glTexCoordPointer(2, GL_FLOAT, 0,  textureCoordinates); }
	else{ glColor4f(0.0f, 0.0f, 1.0f, 1.0f); }
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	//We re-enable the default render state
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_CULL_FACE);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

}
@end