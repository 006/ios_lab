//
//  IndicatorOperation.h
//  Jigsaw
//
//  Created by Chen Li on 10/26/10.
//  Copyright 2010 Chen Li. All rights reserved.
//

#import <Foundation/Foundation.h>

@class JigsawViewController;

@interface IndicatorOperation : NSOperation {
	JigsawViewController* owner;
}

@property (assign) JigsawViewController* owner;

@end
