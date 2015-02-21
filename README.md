# AveStartButtonChanger
Changes the Start Button on Win7

Under Windows 7, the start button is a hardcoded resource in explorer.exe loaded using the
standard LoadImage() function. So, our idea is to patch LoadImage to intercept LoadImage() requests
for the start button and to actually load and return a different image. 
As a bonus, this code will will work for other resources in explorer.exe or other dlls in the same process
as well.

This is the code that actually patches explorer to use our custom LoadImageW function by doing
a pretty default IAT patch on explorer.exe and comctl32.dll (used by lots of default components).

Our custom LoadImage function, called ThunkedLoadImageW(), actually tries
to load images from <currentThemeDir>/<module_with_resource>/<requested_resource>.png/.ico/.cur


#LICENSE
Do what you want with it
