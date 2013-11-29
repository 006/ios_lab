//Screenshot
@interface Screenshot : NSObject {}

+(UIImage*) takeAsUIImage;
+(CCTexture2D*) takeAsTexture2D;
+(NSData*) takeAsPNG;

@end


@implementation Screenshot

+(UIImage*) takeAsUIImage
{
	CCDirector* director = [CCDirector sharedDirector];
	CGSize size = [director displaySizeInPixels];
	
	//Create buffer for pixels
	GLuint bufferLength = size.width * size.height * 4;
	GLubyte* buffer = (GLubyte*)malloc(bufferLength);
	
	//Read Pixels from OpenGL
	glReadPixels(0, 0, size.width, size.height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	
	//Make data provider with data.
	CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, buffer, bufferLength, NULL);
	
	//Configure image
	int bitsPerComponent = 8;
	int bitsPerPixel = 32;
	int bytesPerRow = 4 * size.width;
	CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
	CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
	CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
	CGImageRef iref = CGImageCreate(size.width, size.height, bitsPerComponent, bitsPerPixel, bytesPerRow, colorSpaceRef, bitmapInfo, provider, NULL, NO, renderingIntent);
	CGColorSpaceRelease(colorSpaceRef);
	
	uint32_t* pixels = (uint32_t*)malloc(bufferLength);
	CGContextRef context = CGBitmapContextCreate(pixels, [director winSize].width, [director winSize].height, 8, [director winSize].width * 4, CGImageGetColorSpace(iref), kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
	
	CGContextTranslateCTM(context, 0, size.height);
	CGContextScaleCTM(context, 1.0f, -1.0f);
	
	switch (director.deviceOrientation)
	{
		case CCDeviceOrientationPortrait:
			CGContextRotateCTM(context, CC_DEGREES_TO_RADIANS(0));
			break;
		case CCDeviceOrientationPortraitUpsideDown:
			CGContextRotateCTM(context, CC_DEGREES_TO_RADIANS(180));
			CGContextTranslateCTM(context, -size.width, -size.height);
			break;
		case CCDeviceOrientationLandscapeLeft:
			CGContextRotateCTM(context, CC_DEGREES_TO_RADIANS(-90));
			CGContextTranslateCTM(context, -size.height, 0);
			break;
		case CCDeviceOrientationLandscapeRight:
			CGContextRotateCTM(context, CC_DEGREES_TO_RADIANS(90));
			CGContextTranslateCTM(context, size.width * 0.5f, -size.height);
			break;
	}
	
	CGContextDrawImage(context, CGRectMake(0.0f, 0.0f, size.width, size.height), iref);
	CGImageRef imageRef = CGBitmapContextCreateImage(context);
	UIImage *outputImage = [UIImage imageWithCGImage:imageRef];
	CGImageRelease(imageRef);
	
	//Dealloc
	CGDataProviderRelease(provider);
	CGImageRelease(iref);
	CGContextRelease(context);
	free(buffer);
	free(pixels);
	
	return outputImage;
}

+(CCTexture2D*) takeAsTexture2D
{
	return [[[CCTexture2D alloc] initWithImage:[Screenshot takeAsUIImage]] autorelease];
}

+(NSData*) takeAsPNG
{
	return UIImagePNGRepresentation([Screenshot takeAsUIImage]);
}

@end