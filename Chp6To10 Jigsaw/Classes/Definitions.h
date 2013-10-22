/*
 *  Definitions.h
 *  Jigsaw
 *
 *  Created by Chen Li on 9/23/10.
 *  Copyright 2010 Chen Li. All rights reserved.
 *
 */

#pragma mark Grid
struct Grid {
	int x;
	int y;
};
typedef struct Grid Grid;

CG_INLINE Grid GridMake(int x, int y) {
	Grid p;
	p.x = x;
	p.y = y;
	return p;
}

CG_INLINE bool GridEqualsGrid(Grid p1, Grid p2) {
	return (p1.x==p2.x) && (p1.y==p2.y);
}

CG_INLINE NSString* NSStringFromGrid(Grid g) {
	return NSStringFromCGPoint(CGPointMake(g.x, g.y));
}

CG_INLINE Grid GridFromString(NSString* string) {
	CGPoint p = CGPointFromString(string);
	return GridMake(p.x, p.y);
}

#pragma mark CGPoint extension
CG_INLINE CGPoint CGPointAdd(CGPoint p1, CGPoint p2) {
	return CGPointMake(p1.x+p2.x, p1.y+p2.y);
}

CG_INLINE CGPoint CGPointSub(CGPoint p1, CGPoint p2) {
	return CGPointMake(p1.x-p2.x, p1.y-p2.y);
}

#pragma mark Edges
enum EdgeTypes {
	edgeFlat,
	edgeOut,
	edgeIn
};
typedef enum EdgeTypes EdgeTypes;

CG_INLINE EdgeTypes reverseType(EdgeTypes type) {
	if (type==edgeIn) {
		return edgeOut;
	}
	else if (type==edgeOut) {
		return edgeIn;
	}
	else {
		return edgeFlat;
	}
}

#pragma mark DeskViewState
enum DeskViewState {
	stateNone,
	stateSelectedDeskPiece,
	stateSelectedPoolPiece,
	statePoolToDesk,
	stateSlidePool
};
typedef enum DeskViewState DeskViewState;

#pragma mark constants
#define ARC4RANDOM_MAX      0x10000

// the following is so important to avoid doing NSLog in a release build
// If it's a debug build, then __OPTIMIZE__ is not defined, then NSLog does the normal job;
// if it's a release build, then NSLog will do nothing, thus won't slow down the app
#ifndef __OPTIMIZE__
#    define NSLog(...) NSLog(__VA_ARGS__)
#else
#    define NSLog(...) {}
#endif