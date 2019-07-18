#include "stdafx.h"
#include "PostView.h"
#include <PostViewLib/FEElement.h>
#include <PostViewLib/ColorMap.h>

void Post::Initialize()
{
	FEElementLibrary::InitLibrary();
	ColorMapManager::Initialize();
}
