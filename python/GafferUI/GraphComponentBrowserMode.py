##########################################################################
#  
#  Copyright (c) 2012, John Haddon. All rights reserved.
#  
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are
#  met:
#  
#      * Redistributions of source code must retain the above
#        copyright notice, this list of conditions and the following
#        disclaimer.
#  
#      * Redistributions in binary form must reproduce the above
#        copyright notice, this list of conditions and the following
#        disclaimer in the documentation and/or other materials provided with
#        the distribution.
#  
#      * Neither the name of John Haddon nor the names of
#        any other contributors to this software may be used to endorse or
#        promote products derived from this software without specific prior
#        written permission.
#  
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
#  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
#  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#  
##########################################################################

import re

import Gaffer
import GafferUI

class GraphComponentBrowserMode( GafferUI.BrowserEditor.Mode ) :

	def __init__( self, browser ) :
		
		GafferUI.BrowserEditor.Mode.__init__( self, browser )
		
	def _initialPath( self ) :
	
		scriptWindow = self.browser().ancestor( GafferUI.ScriptWindow )
		scriptNode = scriptWindow.getScript()
		
		return Gaffer.GraphComponentPath(
			scriptNode,
			"/",
			filter = Gaffer.CompoundPathFilter(
					
				filters = [
				
					Gaffer.FileNamePathFilter(
						[ re.compile( "^[^__].*" ) ],
						leafOnly = False,
						userData = {
							"UI" : {
								"label" : "Show hidden",
								"invertEnabled" : True,
							}
						}
					),
					
					Gaffer.InfoPathFilter(
						infoKey = "name",
						matcher = None, # the ui will fill this in
						leafOnly = False,
					),
					
				],				
			),
		)

	def _initialDisplayMode( self ) :
	
		return GafferUI.PathListingWidget.DisplayMode.Tree
		
	def _initialColumns( self ) :
	
		return [ GafferUI.PathListingWidget.defaultNameColumn ]

# not registering automatically, as this mode doesn't make sense in all contexts. instead,
# it can be registered in application startup files (see startup/gui/browser.py).