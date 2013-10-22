//
//  Piece.m
//  Jigsaw
//
//  Created by Chen Li on 9/23/10.
//  Copyright 2010 Chen Li. All rights reserved.
//

#import "Piece.h"


@implementation Piece

@synthesize curGrid;
@synthesize leftEdge,rightEdge,topEdge,bottomEdge;
@synthesize pieceSize,isPoolPiece;

-(id) initWithImage:(UIImage *)aImage leftEdge:(EdgeTypes)l rightEdge:(EdgeTypes)r topEdge:(EdgeTypes)t bottomEdge:(EdgeTypes)b oriGrid:(Grid)aGrid pieceSize:(int)aSize{
	if ( (self=[super initWithImage:aImage]) ) {
		leftEdge = l;
		rightEdge = r;
		topEdge = t;
		bottomEdge = b;
		oriGrid = aGrid;
		pieceSize = aSize;
		
		[self setUserInteractionEnabled:YES];
	}
	
	return self;
}

-(void)setCurGrid:(Grid)aGrid {
	curGrid = aGrid;
}

// 32*32
-(void) toPoolSize {
	float scale = 32.0/(float)pieceSize;
	CGAffineTransform aTransform = CGAffineTransformMakeScale(scale, scale);
	self.transform = aTransform;
}

// pieceSize*pieceSize
-(void) toPieceSize {
	CGAffineTransform aTransform = CGAffineTransformIdentity;
	self.transform = aTransform;
}

// (1.2*pieceSize)*(1.2*pieceSize)
-(void) toLiftSize {
	float scale = 1.2;
	CGAffineTransform aTransform = CGAffineTransformMakeScale(scale, scale);
	self.transform = aTransform;
}

-(NSMutableDictionary*) dict {
	NSMutableDictionary* newPiece = [NSMutableDictionary dictionary];
	[newPiece setValue:[NSNumber numberWithInt:leftEdge] forKey:@"left"];
	[newPiece setValue:[NSNumber numberWithInt:rightEdge] forKey:@"right"];
	[newPiece setValue:[NSNumber numberWithInt:topEdge] forKey:@"top"];
	[newPiece setValue:[NSNumber numberWithInt:bottomEdge] forKey:@"bottom"];
	[newPiece setValue:NSStringFromGrid(oriGrid) forKey:@"oriGrid"];
	[newPiece setValue:NSStringFromGrid(curGrid) forKey:@"curGrid"];
	[newPiece setValue:[NSNumber numberWithBool:isPoolPiece] forKey:@"isPoolPiece"];
	
	return newPiece;
}

-(BOOL) isInPosition {
	return (!isPoolPiece && GridEqualsGrid(curGrid, oriGrid));
}

- (void)dealloc {
    [super dealloc];
}


@end