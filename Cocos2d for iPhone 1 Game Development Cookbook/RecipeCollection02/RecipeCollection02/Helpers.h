float PI = 3.14159;

float degreesToRadians(float d){
	return d * (PI/180);
}
float radiansToDegrees(float r){
	return r * (180/PI);
}
float vectorToRadians(CGPoint vector){
	if(vector.y == 0){ vector.y = 0.000001f; }
	float baseRadians = atan(vector.x/vector.y);
	if(vector.y < 0){ baseRadians += PI; }	//Adjust for -Y
	return baseRadians;
}
CGPoint radiansToVector(float radians){
	return ccp(sin(radians-PI/2), cos(radians-PI/2));
}
float distanceBetweenPoints(CGPoint p1, CGPoint p2){
	return sqrt( pow( (p1.x-p2.x) ,2) + pow( (p1.y-p2.y) ,2) );
}
bool pointIsInRect(CGPoint p, CGRect r){
	bool isInRect = false;
	if( p.x < r.origin.x + r.size.width && 
	   p.x > r.origin.x &&
	   p.y < r.origin.y + r.size.height &&
	   p.y > r.origin.y )
	{
		isInRect = true;
	}
	return isInRect;
}
bool pointIsInCircle(CGPoint p, CGPoint origin, float radius){
	bool isInCircle = false;
	if(distanceBetweenPoints(p, origin) <= radius){
		isInCircle = true;
	}
	return isInCircle;
}
float angleDifference(float angleA, float angleB){
	float diff = fabs(angleA-angleB);
	if(fabs((angleA+360)-angleB) < diff){
		diff = fabs((angleA+360)-angleB);
	}
	if(fabs((angleA-360)-angleB) < diff){
		diff = fabs((angleA-360)-angleB);
	}
	if(fabs(angleA-(angleB+360)) < diff){
		diff = fabs(angleA-(angleB+360));
	}
	if(fabs(angleA-(angleB-360)) < diff){
		diff = fabs(angleA-(angleB-360));
	}
	return diff;
}
float absoluteValue(float num){
	if(num < 0){
		num = num * -1;
	}
	return num;
}
float convertPositionX(CGPoint areaSize, float x){
	return x - (areaSize.x/2);
}
float convertPositionY(CGPoint areaSize, float y){
	return y - (areaSize.y/2);
}
CGPoint midPoint(CGPoint p1, CGPoint p2){
	return ccp( (p1.x+p2.x)/2 , (p1.y+p2.y)/2 );
}