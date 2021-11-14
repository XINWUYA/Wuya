#include "TerminatorApp.h"
/* 程序主入口点，需要保证放在Wuya.h之后 */
#include <Wuya/Application/EntryPoint.h>

void TerminatorApp::Run()
{
	CLIENT_LOG_WARN("Hello! {0}", 5);
}
