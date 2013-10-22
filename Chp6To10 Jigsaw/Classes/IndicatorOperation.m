//
//  IndicatorOperation.m
//  Jigsaw
//
//  Created by Chen Li on 10/26/10.
//  Copyright 2010 Chen Li. All rights reserved.
//

#import "IndicatorOperation.h"
#import "JigsawViewController.h"

@implementation IndicatorOperation

@synthesize owner;

-(void) main {
	[owner hidePopovers];
	[owner startIndicator];
}

@end
