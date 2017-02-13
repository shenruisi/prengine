//
//  AppDelegate.m
//  prengine-debug
//
//  Created by ris on 2/13/17.
//  Copyright © 2017 ris. All rights reserved.
//

#import "AppDelegate.h"
#include "../../package/prengine.h"

@interface AppDelegate ()

@end

pr_id sample_handler(const char *vname){
    if(strcmp(vname, "test") == 0){
        return pr_yes();
    }
    return pr_undefined();
}


@implementation AppDelegate




- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    
    //输入的路由地址
    const char *input1 = "http://taofen8.com/home";
    const char *input2 = "custom-scheme://taofen8.com/detail?goodsId=12345";
    //读取config文件
    FILE *fp = fopen( "/Users/ris/prengine/sample/sample.conf" , "r" );
    fseek(fp , 0L , SEEK_END);
    long size = ftell(fp);
    rewind(fp);
    char *buffer = new char[size];
    fread(buffer , 1 , size , fp);
    buffer[size] = '\0';
    
    PRFILE* prfile = pr_creat(buffer,sample_handler);
    
    PRREWRITE *rewrite = pr_rewrite_matched_creat(prfile, input2);
    
    if (rewrite)
        printf("rewrite output %s.\n",pr_getoutval(rewrite));
    else
        printf("no matched.\n");
    
    delete[] buffer;
    
    return 0;
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
