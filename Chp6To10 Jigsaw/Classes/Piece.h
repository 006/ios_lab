//
//  Piece.h
//  Jigsaw
//
//  Created by Chen Li on 9/23/10.
//  Copyright 2010 Chen Li. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Definitions.h"

@interface Piece : UIImageView {
	Grid oriGrid;			// the correct grid
	Grid curGrid;			// the current grid
	
	EdgeTypes leftEdge, rightEdge, topEdge, bottomEdge;
	int pieceSize;
	
	BOOL isPoolPiece;
}

@property (readwrite) Grid curGrid;
@property (readonly) EdgeTypes leftEdge;
@property (readonly) EdgeTypes rightEdge;
@property (readonly) EdgeTypes topEdge;
@property (readonly) EdgeTypes bottomEdge;
@property (readonly) int pieceSize;
@property (readwrite) BOOL isPoolPiece;

-(id) initWithImage:(UIImage *)aImage leftEdge:(EdgeTypes)l rightEdge:(EdgeTypes)r topEdge:(EdgeTypes)t bottomEdge:(EdgeTypes)b oriGrid:(Grid)aGrid pieceSize:(int)aSize;
-(void) toPoolSize;			// 32*32
-(void) toPieceSize;		// pieceSize*pieceSize
-(void) toLiftSize;			// (1.2*pieceSize)*(1.2*pieceSize)
-(NSMutableDictionary*) dict;				// the wrap up of the piece
-(BOOL) isInPosition;		// check if the piece is already in position

@end
