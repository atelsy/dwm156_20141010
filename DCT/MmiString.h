
#import <Cocoa/Cocoa.h>

@interface callControl(MMIString)


- (BOOL)crackDialString:(NSString*)mmiStr
				forDial:(CString&)strForDial
				forDtmf:(CString&)strFroDtmf;

- (void)checkOnDtmf;

- (TCHAR)getDtmf:(CString&)strDtmf;
- (TCHAR)popDtmf:(CString&)strDtmf;

- (void)clearDtmf;

- (void)closeDtmf;

- (BOOL)isValidDtmf:(const CString*)str;

- (BOOL)setupDtmf;

- (void)beginDtmf;

- (void)sendDtmf;

- (void)onDtmfTimer:(NSTimer*)timer;
- (void)startDtmfTimer;
- (void)stopDtmfTimer;

- (void)onDtmfConfirm:(NSNumber*)code;

- (void)showDtmfWin:(TCHAR)type
		 dtmfString:(CString&)str;

- (void)dismissDtmfWin;

/*
- (NSInteger)showMessageBox:(NSString *)strCaption 
					withMsg:(NSString *)msg 
					forType:(int)type;
*/
@end
