//
//  MyScrollView.m
//  Jigsaw
//
//  Created by Chen Li on 10/23/10.
//  Copyright 2010 Chen Li. All rights reserved.
//

#import "MyScrollView.h"


@implementation MyScrollView

// forward the touch event; UIScrollView absorb the touch events by default
-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	[[self nextResponder] touchesBegan:touches withEvent:event];
	[super touchesBegan:touches withEvent:event];
}

-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	[[self nextResponder] touchesMoved:touches withEvent:event];
	[super touchesMoved:touches withEvent:event];
}

-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	NSLog(@"ended");
	[[self nextResponder] touchesEnded:touches withEvent:event];
	[super touchesEnded:touches withEvent:event];
}


@end
