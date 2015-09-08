#pragma once
#include "..\Core\Bitmap.h"
#include "..\..\dependencies\gl\include\glew.h"
#include "..\..\dependencies\assimp\include\assimp\scene.h"
#include <map>

class TextureManager 
{
public:
	~TextureManager();

	static TextureManager& GetInstance();

	// For assimp models only.
	std::vector<std::string> makeFullTexturePaths(const aiMaterial* material, std::string modelPath);

	bool TextureManager::loadTexture(std::string texturePath);

	GLuint getTextureHandle(std::string texturePath)
	{ 
		return m_textureIdMap[texturePath]; 
	}

protected:
	TextureManager();

private:
	// Note: Map values can be overwritten with the []-operator.
	std::map<std::string, GLuint> m_textureIdMap;   //!< Contains texture paths to texture handle
	bool						  m_initialized;	//!< True if the texture manager is ready
};