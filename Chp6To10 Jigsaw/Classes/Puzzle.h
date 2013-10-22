//
//  Puzzle.h
//  Jigsaw
//
//  Created by Chen Li on 9/23/10.
//  Copyright 2010 Chen Li. All rights reserved.
//

// Puzzle and Piece are the data classes for this app. 

#import <Foundation/Foundation.h>

@class Piece;

@interface Puzzle : NSObject {
	NSMutableArray* poolPieces;
	NSMutableArray* tablePieces;
	
	int timePlayed;
	int pieceSize;
	int numCol;
	int numRow;
	
	UIImage* image;
	NSString* fileName;		// only name and extension, no path
	NSString* imagePath;
}

+(NSMutableDictionary*)genDictFromImage:(NSString*)imagePath pieceSize:(int)theSize;
-(id)initWithDict:(NSDictionary*)dict;
-(NSMutableDictionary*)dict;
-(BOOL)isFinished;

@property (retain) NSMutableArray* poolPieces;
@property (retain) NSMutableArray* tablePieces;
@property (readwrite) int pieceSize;
@property (readwrite) int numCol;
@property (readwrite) int numRow;
@property (retain) UIImage* image;
@property (retain) NSString* fileName;
@property (retain) NSString* imagePath;
@property (readwrite) int timePlayed;

@end
