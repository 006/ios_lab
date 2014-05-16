//
//  OnMeAppDelegate.h
//  Quiz OnMe
//
//  Created by ttron kidman on 14-5-16.
//  Copyright (c) 2014年 Tsst Corp. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface OnMeAppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@property (readonly, strong, nonatomic) NSManagedObjectContext *managedObjectContext;
@property (readonly, strong, nonatomic) NSManagedObjectModel *managedObjectModel;
@property (readonly, strong, nonatomic) NSPersistentStoreCoordinator *persistentStoreCoordinator;

- (void)saveContext;
- (NSURL *)applicationDocumentsDirectory;

@end
