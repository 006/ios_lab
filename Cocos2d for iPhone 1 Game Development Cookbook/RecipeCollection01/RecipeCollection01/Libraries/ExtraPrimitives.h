/* Create a solid circle */
void ccDrawSolidCircle( CGPoint center, float r, float a, NSUInteger segs, BOOL drawLineToCenter)
{
	int additionalSegment = 1;
	if (drawLineToCenter)
		additionalSegment++;

	const float coef = 2.0f * (float)M_PI/segs;
	
	GLfloat *vertices = calloc( sizeof(GLfloat)*2*(segs+2), 1);
	if( ! vertices )
		return;

	for(NSUInteger i=0;i<=segs;i++)
	{
		float rads = i*coef;
		GLfloat j = r * cosf(rads + a) + center.x;
		GLfloat k = r * sinf(rads + a) + center.y;
		
		vertices[i*2] = j * CC_CONTENT_SCALE_FACTOR();
		vertices[i*2+1] =k * CC_CONTENT_SCALE_FACTOR();
	}
	vertices[(segs+1)*2] = center.x * CC_CONTENT_SCALE_FACTOR();
	vertices[(segs+1)*2+1] = center.y * CC_CONTENT_SCALE_FACTOR();
		
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glVertexPointer(2, GL_FLOAT, 0, vertices);	
	glDrawArrays(GL_TRIANGLE_FAN, 0, segs+additionalSegment);
	
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);	
	
	free( vertices );
}