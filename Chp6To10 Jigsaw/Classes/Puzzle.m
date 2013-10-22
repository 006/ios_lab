//
//  Puzzle.m
//  Jigsaw
//
//  Created by Chen Li on 9/23/10.
//  Copyright 2010 Chen Li. All rights reserved.
//

#import "Puzzle.h"
#import "Piece.h"

@implementation Puzzle

@synthesize poolPieces, tablePieces, pieceSize, numCol, numRow, timePlayed;
@synthesize image, fileName, imagePath;

-(id)initWithDict:(NSMutableDictionary*)dict {
	if ((self=[super init])) {
		poolPieces = [[NSMutableArray alloc] init];
		tablePieces = [[NSMutableArray alloc] init];
		
		// get dict info
		self.imagePath = [dict valueForKey:@"imagePath"];
		UIImage* aImage = [UIImage imageWithContentsOfFile:self.imagePath];
		pieceSize = [[dict valueForKey:@"pieceSize"] intValue];
		numCol = [[dict valueForKey:@"numCol"] intValue];
		numRow = [[dict valueForKey:@"numRow"] intValue];
		NSMutableArray* arrDictPieces = [dict valueForKey:@"pieces"];
		timePlayed = [[dict valueForKey:@"timePlayed"] intValue];
		
		// image
		self.image = aImage;
		// file name
		self.fileName = [dict valueForKey:@"fileName"];
		
		// line width used for stroking the edge of pieces
		int lineWidth = 2;
		// the width of the second part of the bone shape
		int gw = 0.2*pieceSize;
		// image size
		int w = aImage.size.width;
		int h = aImage.size.height;
		// piece size
		int fw = pieceSize;
		int fh = pieceSize;
		
		// color space
		CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
		
		for (int i=0; i<[arrDictPieces count]; i++) {
			// dict for the piece
			NSMutableDictionary* dictForPiece = [arrDictPieces objectAtIndex:i];
			// edges
			int left = [[dictForPiece valueForKey:@"left"] intValue];
			int right = [[dictForPiece valueForKey:@"right"] intValue];
			int top = [[dictForPiece valueForKey:@"top"] intValue];
			int bottom = [[dictForPiece valueForKey:@"bottom"] intValue];
			// grid
			Grid oriGrid = GridFromString([dictForPiece valueForKey:@"oriGrid"]);
			int x = oriGrid.x;
			int y = oriGrid.y;
			// generate image for the piece, in 5 steps
			
			// 1. create the context, reverse it upside down; after the translate, origin is at top-left corner
			CGContextRef context = CGBitmapContextCreate(NULL, w+3.2*gw, h+3.2*gw, 8, 6 * (w), colorSpace, kCGImageAlphaPremultipliedFirst);
			CGContextTranslateCTM(context, 0, h+3.2*gw);
			CGContextScaleCTM(context, 1.0, -1.0);
			// 2. create the path for clipping; clipping will affect the subsequent drawings
			CGContextBeginPath(context);
			// create the path
			// top left corner, note the transition on y
			CGContextMoveToPoint(context, x*fw+1.6*gw, (y)*fh+1.6*gw);	
			// TOP edge: 1st and 3rd part are just line segments, but the 2nd part varies
			int cx = (x+0.5)*fw+1.6*gw;
			int cy = (y)*fh+1.6*gw;
			CGContextAddLineToPoint(context, cx-0.5*gw, cy);		// 1st part
			switch (top) {													// 2nd part
				case edgeFlat:
					CGContextAddLineToPoint(context, cx+0.5*gw, cy);
					break;
				case edgeIn:
					CGContextAddArcToPoint(context, cx-1.2*gw, cy+1.2*gw, cx, cy+1.6*gw, 0.7*gw);
					CGContextAddArcToPoint(context, cx, cy+1.6*gw, cx+1.2*gw, cy+1.2*gw, 1.2*gw);
					CGContextAddArcToPoint(context, cx+1.2*gw, cy+1.2*gw, cx+0.5*gw, cy, 0.7*gw);
					CGContextAddLineToPoint(context, cx+0.5*gw, cy);
					break;
				case edgeOut:
					CGContextAddArcToPoint(context, cx-1.2*gw, cy-1.2*gw, cx, cy-1.6*gw, 0.7*gw);
					CGContextAddArcToPoint(context, cx, cy-1.6*gw, cx+1.2*gw, cy-1.2*gw, 1.2*gw);
					CGContextAddArcToPoint(context, cx+1.2*gw, cy-1.2*gw, cx+0.5*gw, cy, 0.7*gw);
					CGContextAddLineToPoint(context, cx+0.5*gw, cy);
					break;
				default:
					break;
			}
			CGContextAddLineToPoint(context, (x+1.0)*fw+1.6*gw, (y)*fh+1.6*gw);		// 3rd part
			// RIGHT edge: 1st and 3rd part are just line segments, but the 2nd part varies
			cx = (x+1.0)*fw+1.6*gw;
			cy = (y+0.5)*fh+1.6*gw;
			CGContextAddLineToPoint(context, cx, cy-0.5*gw);		// 1st part
			switch (right) {													// 2nd part
				case edgeFlat:
					CGContextAddLineToPoint(context, cx, cy+0.5*gw);
					break;
				case edgeIn:
					CGContextAddArcToPoint(context, cx-1.2*gw, cy-1.2*gw, cx-1.6*gw, cy, 0.7*gw);
					CGContextAddArcToPoint(context, cx-1.6*gw, cy, cx-1.2*gw, cy+1.2*gw, 1.2*gw);
					CGContextAddArcToPoint(context, cx-1.2*gw, cy+1.2*gw, cx, cy+0.5*gw, 0.7*gw);
					CGContextAddLineToPoint(context, cx, cy+0.5*gw);
					break;
				case edgeOut:
					CGContextAddArcToPoint(context, cx+1.2*gw, cy-1.2*gw, cx+1.6*gw, cy, 0.7*gw);
					CGContextAddArcToPoint(context, cx+1.6*gw, cy, cx+1.2*gw, cy+1.2*gw, 1.2*gw);
					CGContextAddArcToPoint(context, cx+1.2*gw, cy+1.2*gw, cx, cy+0.5*gw, 0.7*gw);
					CGContextAddLineToPoint(context, cx, cy+0.5*gw);
					break;
				default:
					break;
			}
			CGContextAddLineToPoint(context, (x+1.0)*fw+1.6*gw, (y+1.0)*fh+1.6*gw);		// 3rd part
			// BOTTOM edge: 1st and 3rd part are just line segments, but the 2nd part varies
			cx = (x+0.5)*fw+1.6*gw;
			cy = (y+1.0)*fh+1.6*gw;
			CGContextAddLineToPoint(context, cx+0.5*gw, cy);		// 1st part
			switch (bottom) {													// 2nd part
				case edgeFlat:
					CGContextAddLineToPoint(context, cx-0.5*gw, cy);
					break;
				case edgeIn:
					CGContextAddArcToPoint(context, cx+1.2*gw, cy-1.2*gw, cx, cy-1.6*gw, 0.7*gw);
					CGContextAddArcToPoint(context, cx, cy-1.6*gw, cx-1.2*gw, cy-1.2*gw, 1.2*gw);
					CGContextAddArcToPoint(context, cx-1.2*gw, cy-1.2*gw, cx-0.5*gw, cy, 0.7*gw);
					CGContextAddLineToPoint(context, cx-0.5*gw, cy);
					break;
				case edgeOut:
					CGContextAddArcToPoint(context, cx+1.2*gw, cy+1.2*gw, cx, cy+1.6*gw, 0.7*gw);
					CGContextAddArcToPoint(context, cx, cy+1.6*gw, cx-1.2*gw, cy+1.2*gw, 1.2*gw);
					CGContextAddArcToPoint(context, cx-1.2*gw, cy+1.2*gw, cx-0.5*gw, cy, 0.7*gw);
					CGContextAddLineToPoint(context, cx-0.5*gw, cy);
					break;
				default:
					break;
			}
			CGContextAddLineToPoint(context, (x)*fw+1.6*gw, (y+1.0)*fh+1.6*gw);		// 3rd part
			// LEFT edge: 1st and 3rd part are just line segments, but the 2nd part varies
			cx = (x)*fw+1.6*gw;
			cy = (y+0.5)*fh+1.6*gw;
			CGContextAddLineToPoint(context, cx, cy+0.5*gw);		// 1st part
			switch (left) {													// 2nd part
				case edgeFlat:
					CGContextAddLineToPoint(context, cx, cy-0.5*gw);
					break;
				case edgeIn:
					CGContextAddArcToPoint(context, cx+1.2*gw, cy+1.2*gw, cx+1.6*gw, cy, 0.7*gw);
					CGContextAddArcToPoint(context, cx+1.6*gw, cy, cx+1.2*gw, cy-1.2*gw, 1.2*gw);
					CGContextAddArcToPoint(context, cx+1.2*gw, cy-1.2*gw, cx, cy-0.5*gw, 0.7*gw);
					CGContextAddLineToPoint(context, cx, cy-0.5*gw);
					break;
				case edgeOut:
					CGContextAddArcToPoint(context, cx-1.2*gw, cy+1.2*gw, cx-1.6*gw, cy, 0.7*gw);
					CGContextAddArcToPoint(context, cx-1.6*gw, cy, cx-1.2*gw, cy-1.2*gw, 1.2*gw);
					CGContextAddArcToPoint(context, cx-1.2*gw, cy-1.2*gw, cx, cy-0.5*gw, 0.7*gw);
					CGContextAddLineToPoint(context, cx, cy-0.5*gw);
					break;
				default:
					break;
			}
			CGContextAddLineToPoint(context, x*fw+1.6*gw, y*fh+1.6*gw);		// 3rd part
			CGContextClosePath(context);
			// clip
			CGContextClip(context);
			
			// 3. draw the original image - note we need to translate it temporarily
			CGContextSaveGState(context);
			CGContextTranslateCTM(context, 0, h+3.2*gw);
			CGContextScaleCTM(context, 1.0, -1.0);
			CGContextDrawImage(context, CGRectMake(1.6*gw,1.6*gw,w+3.2*gw,h+3.2*gw), aImage.CGImage);
			CGContextRestoreGState(context);
			
			// 4. re-create the path and stroke
			// create the path
			// top left corner, note the transition on y
			CGContextMoveToPoint(context, x*fw+1.6*gw, (y)*fh+1.6*gw);	
			// TOP edge: 1st and 3rd part are just line segments, but the 2nd part varies
			cx = (x+0.5)*fw+1.6*gw;
			cy = (y)*fh+1.6*gw;
			CGContextAddLineToPoint(context, cx-0.5*gw, cy);		// 1st part
			switch (top) {													// 2nd part
				case edgeFlat:
					CGContextAddLineToPoint(context, cx+0.5*gw, cy);
					break;
				case edgeIn:
					CGContextAddArcToPoint(context, cx-1.2*gw, cy+1.2*gw, cx, cy+1.6*gw, 0.7*gw);
					CGContextAddArcToPoint(context, cx, cy+1.6*gw, cx+1.2*gw, cy+1.2*gw, 1.2*gw);
					CGContextAddArcToPoint(context, cx+1.2*gw, cy+1.2*gw, cx+0.5*gw, cy, 0.7*gw);
					CGContextAddLineToPoint(context, cx+0.5*gw, cy);
					break;
				case edgeOut:
					CGContextAddArcToPoint(context, cx-1.2*gw, cy-1.2*gw, cx, cy-1.6*gw, 0.7*gw);
					CGContextAddArcToPoint(context, cx, cy-1.6*gw, cx+1.2*gw, cy-1.2*gw, 1.2*gw);
					CGContextAddArcToPoint(context, cx+1.2*gw, cy-1.2*gw, cx+0.5*gw, cy, 0.7*gw);
					CGContextAddLineToPoint(context, cx+0.5*gw, cy);
					break;
				default:
					break;
			}
			CGContextAddLineToPoint(context, (x+1.0)*fw+1.6*gw, (y)*fh+1.6*gw);		// 3rd part
			// RIGHT edge: 1st and 3rd part are just line segments, but the 2nd part varies
			cx = (x+1.0)*fw+1.6*gw;
			cy = (y+0.5)*fh+1.6*gw;
			CGContextAddLineToPoint(context, cx, cy-0.5*gw);		// 1st part
			switch (right) {													// 2nd part
				case edgeFlat:
					CGContextAddLineToPoint(context, cx, cy+0.5*gw);
					break;
				case edgeIn:
					CGContextAddArcToPoint(context, cx-1.2*gw, cy-1.2*gw, cx-1.6*gw, cy, 0.7*gw);
					CGContextAddArcToPoint(context, cx-1.6*gw, cy, cx-1.2*gw, cy+1.2*gw, 1.2*gw);
					CGContextAddArcToPoint(context, cx-1.2*gw, cy+1.2*gw, cx, cy+0.5*gw, 0.7*gw);
					CGContextAddLineToPoint(context, cx, cy+0.5*gw);
					break;
				case edgeOut:
					CGContextAddArcToPoint(context, cx+1.2*gw, cy-1.2*gw, cx+1.6*gw, cy, 0.7*gw);
					CGContextAddArcToPoint(context, cx+1.6*gw, cy, cx+1.2*gw, cy+1.2*gw, 1.2*gw);
					CGContextAddArcToPoint(context, cx+1.2*gw, cy+1.2*gw, cx, cy+0.5*gw, 0.7*gw);
					CGContextAddLineToPoint(context, cx, cy+0.5*gw);
					break;
				default:
					break;
			}
			CGContextAddLineToPoint(context, (x+1.0)*fw+1.6*gw, (y+1.0)*fh+1.6*gw);		// 3rd part
			// BOTTOM edge: 1st and 3rd part are just line segments, but the 2nd part varies
			cx = (x+0.5)*fw+1.6*gw;
			cy = (y+1.0)*fh+1.6*gw;
			CGContextAddLineToPoint(context, cx+0.5*gw, cy);		// 1st part
			switch (bottom) {													// 2nd part
				case edgeFlat:
					CGContextAddLineToPoint(context, cx-0.5*gw, cy);
					break;
				case edgeIn:
					CGContextAddArcToPoint(context, cx+1.2*gw, cy-1.2*gw, cx, cy-1.6*gw, 0.7*gw);
					CGContextAddArcToPoint(context, cx, cy-1.6*gw, cx-1.2*gw, cy-1.2*gw, 1.2*gw);
					CGContextAddArcToPoint(context, cx-1.2*gw, cy-1.2*gw, cx-0.5*gw, cy, 0.7*gw);
					CGContextAddLineToPoint(context, cx-0.5*gw, cy);
					break;
				case edgeOut:
					CGContextAddArcToPoint(context, cx+1.2*gw, cy+1.2*gw, cx, cy+1.6*gw, 0.7*gw);
					CGContextAddArcToPoint(context, cx, cy+1.6*gw, cx-1.2*gw, cy+1.2*gw, 1.2*gw);
					CGContextAddArcToPoint(context, cx-1.2*gw, cy+1.2*gw, cx-0.5*gw, cy, 0.7*gw);
					CGContextAddLineToPoint(context, cx-0.5*gw, cy);
					break;
				default:
					break;
			}
			CGContextAddLineToPoint(context, (x)*fw+1.6*gw, (y+1.0)*fh+1.6*gw);		// 3rd part
			// LEFT edge: 1st and 3rd part are just line segments, but the 2nd part varies
			cx = (x)*fw+1.6*gw;
			cy = (y+0.5)*fh+1.6*gw;
			CGContextAddLineToPoint(context, cx, cy+0.5*gw);		// 1st part
			switch (left) {													// 2nd part
				case edgeFlat:
					CGContextAddLineToPoint(context, cx, cy-0.5*gw);
					break;
				case edgeIn:
					CGContextAddArcToPoint(context, cx+1.2*gw, cy+1.2*gw, cx+1.6*gw, cy, 0.7*gw);
					CGContextAddArcToPoint(context, cx+1.6*gw, cy, cx+1.2*gw, cy-1.2*gw, 1.2*gw);
					CGContextAddArcToPoint(context, cx+1.2*gw, cy-1.2*gw, cx, cy-0.5*gw, 0.7*gw);
					CGContextAddLineToPoint(context, cx, cy-0.5*gw);
					break;
				case edgeOut:
					CGContextAddArcToPoint(context, cx-1.2*gw, cy+1.2*gw, cx-1.6*gw, cy, 0.7*gw);
					CGContextAddArcToPoint(context, cx-1.6*gw, cy, cx-1.2*gw, cy-1.2*gw, 1.2*gw);
					CGContextAddArcToPoint(context, cx-1.2*gw, cy-1.2*gw, cx, cy-0.5*gw, 0.7*gw);
					CGContextAddLineToPoint(context, cx, cy-0.5*gw);
					break;
				default:
					break;
			}
			CGContextAddLineToPoint(context, x*fw+1.6*gw, y*fh+1.6*gw);		// 3rd part
			CGContextClosePath(context);
			// stroke
			CGContextSetStrokeColorSpace(context, colorSpace);
			CGFloat strokeColor[4]    = {0.7,0.7,0.7,1.0};
			CGContextSetStrokeColor(context, strokeColor);
			CGContextSetLineWidth(context, lineWidth);
			CGContextStrokePath(context);						// this will clear the current path
			
			// get the image
			CGImageRef largeImageMasked = CGBitmapContextCreateImage(context);
			
			// 5. get only the specific region
			CGImageRef imageMasked = CGImageCreateWithImageInRect(largeImageMasked, CGRectMake(x*fw, (y)*fh, fw+3.2*gw, fh+3.2*gw));
			CGImageRelease(largeImageMasked);
			//CGImageRef imageMasked = largeImageMasked;
			CGContextRelease(context);
			
			UIImage* newImage = [UIImage imageWithCGImage:imageMasked];
			CGImageRelease(imageMasked);
			//NSLog(@"%@",NSStringFromCGSize(newImage.size));
			
			// create a piece
			Piece* newPiece = [[Piece alloc] initWithImage:newImage\
												  leftEdge:left rightEdge:right topEdge:top bottomEdge:bottom oriGrid:GridMake(x, y) pieceSize:pieceSize];
			newPiece.isPoolPiece = [[dictForPiece valueForKey:@"isPoolPiece"] boolValue];
			newPiece.curGrid = GridFromString([dictForPiece valueForKey:@"curGrid"]);
			[newPiece autorelease];
			//newPiece.center = CGPointMake(fw/2+x*fw, fh/2+y*fh);
			
			// add to pool or table
			if (newPiece.isPoolPiece) {
				[poolPieces addObject:newPiece];
			}
			else {
				[tablePieces addObject:newPiece];
			}

		}
		// release color space
		CGColorSpaceRelease(colorSpace);
	}

	return self;
}

+(NSMutableDictionary*)genDictFromImage:(NSString*)imagePath pieceSize:(int)size {
	// get image
	UIImage* aImage = [UIImage imageWithContentsOfFile:imagePath];
	int w = aImage.size.width;
	int h = aImage.size.height;
	int maxX = w/size;
	int maxY = h/size;
	
	// file name for the dict
	NSString* fileName = [[[imagePath lastPathComponent] stringByDeletingPathExtension] stringByAppendingFormat:@".plist"];
	
	// the dict to return
	NSMutableDictionary* dictPuzzle = [NSMutableDictionary dictionary];
	
	NSMutableDictionary* mapPieces[maxX][maxY];
	NSMutableArray* arrPieces = [NSMutableArray array];
	
	// keep in mind, we want the origin to be on the top-left
	for (int x = 0; x <= maxX-1; x++) {
		for (int y = 0; y <= maxY-1; y++) {
			// generate edge types
			EdgeTypes left,right,top,bottom;
			// left edge
			if (x==0) {
				left = edgeFlat;
			}
			else {
				NSMutableDictionary* neighborPiece = mapPieces[x-1][y];
				left = reverseType([[neighborPiece valueForKey:@"right"] intValue]);
			}
			// right edge
			if (x==maxX-1) {
				right = edgeFlat;
			}
			else {
				// half of the chance edgeOut, the other half edgeIn
				float val = (float)arc4random()/ARC4RANDOM_MAX/ARC4RANDOM_MAX;	// [0,1)
				if (val>0.5) {
					right = edgeOut;
				}
				else {
					right = edgeIn;
				}
			}
			// top edge
			if (y==0) {
				top = edgeFlat;
			}
			else {
				NSMutableDictionary* neighborPiece = mapPieces[x][y-1];
				top = reverseType([[neighborPiece valueForKey:@"bottom"] intValue]);
			}
			// bottom edge
			if (y==maxY-1) {
				bottom = edgeFlat;
			}
			else {
				// half of the chance edgeOut, the other half edgeIn
				float val = (float)arc4random()/ARC4RANDOM_MAX/ARC4RANDOM_MAX;	// [0,1)
				if (val>0.5) {
					bottom = edgeOut;
				}
				else {
					bottom = edgeIn;
				}
			}
			NSLog(@"x:%d,y:%d,t:%d,b:%d,l:%d,r:%d",x,y,top,bottom,left,right);
			// create a piece
			NSMutableDictionary* newPiece = [NSMutableDictionary dictionary];
			[newPiece setValue:[NSNumber numberWithInt:left] forKey:@"left"];
			[newPiece setValue:[NSNumber numberWithInt:right] forKey:@"right"];
			[newPiece setValue:[NSNumber numberWithInt:top] forKey:@"top"];
			[newPiece setValue:[NSNumber numberWithInt:bottom] forKey:@"bottom"];
			[newPiece setValue:NSStringFromGrid(GridMake(x, y)) forKey:@"oriGrid"];
			[newPiece setValue:NSStringFromGrid(GridMake(0, 0)) forKey:@"curGrid"];
			[newPiece setValue:[NSNumber numberWithBool:YES] forKey:@"isPoolPiece"];
			// add to mapPieces
			mapPieces[x][y] = newPiece;
			
			// insert the current piece into a random place of the pool
			float N = [arrPieces count];							// current piece
			float val = (float)arc4random()/ARC4RANDOM_MAX/ARC4RANDOM_MAX;	// [0,1)
			int pos = floor(val*(N+1));
			[arrPieces insertObject:newPiece atIndex:pos];
			//NSLog(@"%d",pos);
		}
	}
	
	[dictPuzzle setValue:[NSNumber numberWithInt:maxY] forKey:@"numRow"];
	[dictPuzzle setValue:[NSNumber numberWithInt:maxX] forKey:@"numCol"];
	[dictPuzzle setValue:[NSNumber numberWithInt:size] forKey:@"pieceSize"];
	[dictPuzzle setValue:[NSNumber numberWithInt:0] forKey:@"timePlayed"];
	[dictPuzzle setValue:imagePath forKey:@"imagePath"];
	[dictPuzzle setValue:arrPieces forKey:@"pieces"];
	[dictPuzzle setValue:fileName forKey:@"fileName"];
	
	return dictPuzzle;
}

-(NSMutableDictionary*)dict {
	// the dict to return
	NSMutableDictionary* dictPuzzle = [NSMutableDictionary dictionary];
	// pieces
	NSMutableArray* arrPieces = [NSMutableArray array];
	for (Piece* aPiece in poolPieces) {
		[arrPieces addObject:[aPiece dict]];
	}
	for (Piece* aPiece in tablePieces) {
		[arrPieces addObject:[aPiece dict]];
	}
	// values
	[dictPuzzle setValue:[NSNumber numberWithInt:self.numRow] forKey:@"numRow"];
	[dictPuzzle setValue:[NSNumber numberWithInt:self.numCol] forKey:@"numCol"];
	[dictPuzzle setValue:[NSNumber numberWithInt:self.pieceSize] forKey:@"pieceSize"];
	[dictPuzzle setValue:[NSNumber numberWithInt:timePlayed] forKey:@"timePlayed"];
	[dictPuzzle setValue:self.imagePath forKey:@"imagePath"];
	[dictPuzzle setValue:arrPieces forKey:@"pieces"];
	[dictPuzzle setValue:fileName forKey:@"fileName"];
	
	return dictPuzzle;
}

// check if the puzzle is finished
-(BOOL)isFinished {
	BOOL result = NO;
	// first condition: no pool pieces
	if ([poolPieces count]==0) {
		int i=0;
		while ([[tablePieces objectAtIndex:i] isInPosition]) {
			i++;
			if (i==[tablePieces count]) {
				result = YES;
				break;
			}
		}
	}
	
	return result;
}

-(void)dealloc {
	[poolPieces release];
	[tablePieces release];
	[image release];
	[fileName release];
	[imagePath release];
	
	[super dealloc];
}

@end
