#import <Foundation/Foundation.h>

/* This returns the full absolute path to a specified file in the bundle */
NSString* getActualPath( NSString* file )
{
	NSArray* path = [file componentsSeparatedByString: @"."];
	NSString* actualPath = [[NSBundle mainBundle] pathForResource: [path objectAtIndex: 0] ofType: [path objectAtIndex: 1]];
		
	return actualPath;
}