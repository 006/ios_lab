//CCMoveByCustom
@interface CCMoveByCustom : CCMoveBy
{}
-(void) update: (ccTime) t;
@end

@implementation CCMoveByCustom
-(void) update: (ccTime) t {	
	//Here we neglect to change something with a zero delta.
	if(delta.x == 0){
		[target_ setPosition: ccp( [(CCNode*)target_ position].x, (startPosition.y + delta.y * t ) )];
	}else if(delta.y == 0){
		[target_ setPosition: ccp( (startPosition.x + delta.x * t ), [(CCNode*)target_ position].y )];
	}else{
		[target_ setPosition: ccp( (startPosition.x + delta.x * t ), (startPosition.y + delta.y * t ) )];
	}
}
@end