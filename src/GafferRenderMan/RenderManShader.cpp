//////////////////////////////////////////////////////////////////////////
//  
//  Copyright (c) 2013, Image Engine Design Inc. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//  
//      * Redistributions of source code must retain the above
//        copyright notice, this list of conditions and the following
//        disclaimer.
//  
//      * Redistributions in binary form must reproduce the above
//        copyright notice, this list of conditions and the following
//        disclaimer in the documentation and/or other materials provided with
//        the distribution.
//  
//      * Neither the name of John Haddon nor the names of
//        any other contributors to this software may be used to endorse or
//        promote products derived from this software without specific prior
//        written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
//  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//  
//////////////////////////////////////////////////////////////////////////

#include "IECore/CachedReader.h"

#include "Gaffer/CompoundDataPlug.h"
#include "Gaffer/TypedPlug.h"

#include "GafferRenderMan/RenderManShader.h"

using namespace IECore;
using namespace Gaffer;
using namespace GafferScene;
using namespace GafferRenderMan;

IE_CORE_DEFINERUNTIMETYPED( RenderManShader );

RenderManShader::RenderManShader( const std::string &name )
	:	GafferScene::Shader( name )
{
	addChild( new StringPlug( "__shaderName" ) );
	addChild( new Plug( "out", Plug::Out ) );
}

RenderManShader::~RenderManShader()
{
}

void RenderManShader::loadShader( const std::string &shaderName )
{
	IECore::ConstShaderPtr shader = runTimeCast<const IECore::Shader>( shaderLoader()->read( shaderName + ".sdl" ) );
		
	for( CompoundDataMap::const_iterator it = shader->parameters().begin(), eIt = shader->parameters().end(); it != eIt; it++ )
	{
		ValuePlugPtr valuePlug = CompoundDataPlug::createPlugFromData(
			it->first,
			Plug::In,
			Plug::Default | Plug::Dynamic,
			it->second
		);
		parametersPlug()->addChild( valuePlug );
	}
	
	getChild<StringPlug>( "__shaderName" )->setValue( shaderName );
}

void RenderManShader::shaderHash( IECore::MurmurHash &h ) const
{
	Shader::shaderHash( h );
	getChild<StringPlug>( "__shaderName" )->hash( h );
}

IECore::ShaderPtr RenderManShader::shader( NetworkBuilder &network ) const
{
	ShaderPtr result = new IECore::Shader( getChild<StringPlug>( "__shaderName" )->getValue(), "ri:surface" );
	for( InputValuePlugIterator it( parametersPlug() ); it!=it.end(); it++ )
	{
		result->parameters()[(*it)->getName()] = CompoundDataPlug::extractDataFromPlug( *it );
	}
	return result;
}

IECore::CachedReader *RenderManShader::shaderLoader()
{
	static CachedReaderPtr g_loader;
	if( !g_loader )
	{
		const char *sp = getenv( "DL_SHADERS_PATH" );
		sp = sp ? sp : "";
		g_loader = new CachedReader( SearchPath( sp, ":" ), 10 * 1024 * 1024 );
	}
	return g_loader.get();
}