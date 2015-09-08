#pragma once
#include "Logger.h"
#include "TextureManager.h"
#include "OGLErrorHandling.h"

static std::shared_ptr<TextureManager> Instance;

TextureManager::TextureManager()
{
	m_initialized = false;
}


TextureManager::~TextureManager()
{
}

TextureManager& TextureManager::GetInstance()
{
	if(Instance.get() == nullptr)
	{
		Instance = std::shared_ptr<TextureManager>(new TextureManager());
	}
	return *Instance.get();
}

// Assimp specific function. Add later aiTextureType parameter.
std::vector<std::string> TextureManager::makeFullTexturePaths(const aiMaterial* material, std::string modelPath)
{
	std::vector<std::string> fullTexturePaths;

	int localTextureCount = 0;
    aiString path;  // filename
 
    aiReturn texFound = material->GetTexture(aiTextureType_DIFFUSE, localTextureCount, &path);
    while (texFound == AI_SUCCESS) 
	{
        // fill map with textures, OpenGL image ids set to 0
		std::string filename = path.data;
		unsigned found_double_backspace = filename.find_first_of("'\\' | '\'");
		if(found_double_backspace != std::string::npos)
		{
			std::string filename_tmp = filename.substr(0,found_double_backspace);
			filename_tmp.append("/");
			filename_tmp.append(filename.substr(found_double_backspace+1));
			filename = filename_tmp;
		}

		std::string total_path = modelPath;
		total_path.append(filename);

		// Store the total texture path already here in the map, to avoid redundancies.
		// If-case prevents from overwriting the handle, because if the total texture path is already known to the map it also has a 
		if(m_textureIdMap.find(total_path) == m_textureIdMap.end())
		{
			m_textureIdMap[total_path]=0;
		}

		fullTexturePaths.push_back(total_path);

        // more textures?
        localTextureCount++;
		texFound = material->GetTexture(aiTextureType_DIFFUSE, localTextureCount, &path);
	}

	return fullTexturePaths;
}

bool TextureManager::loadTexture(std::string totalTexturePath)
{
	// Texture already loaded once.
	if (m_textureIdMap.find(totalTexturePath) != m_textureIdMap.end())
	{
		if (m_textureIdMap[totalTexturePath] != 0)
		{
			return true;
		}
	}

	Bitmap image;

	bool success = image.loadFile(totalTexturePath);
	
	//isTranparent = image.isTransparent();

	// check image format
	//std::string image_format = "";
	//unsigned found_dot = totalTexturePath.find_last_of(".");
	//if(found_dot != std::string::npos)
	//{
	//	image_format = totalTexturePath.substr(found_dot);
	//}

	if (success) {
        /* Create and load textures to OpenGL */
		GLuint textureHandle = 0; 
		glGenTextures(1, &textureHandle);
		glBindTexture(GL_TEXTURE_2D, textureHandle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		if(image.getBitsPerPixel() == 24)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.getWidth(), 
				image.getHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE, 
				image.getData());
		}
		else if (image.getBitsPerPixel() == 32)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getWidth(),
				image.getHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE,
				image.getData());
		}

		//if(image_format.compare(".png") == 0)
		//{
		//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getWidth(),
		//		image.getHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE,
		//		image.getData());
		//}
		//else if(image_format.compare(".tga") == 0||image_format.compare(".jpg") == 0)
		//{
		//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.getWidth(), 
		//		image.getHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE, 
		//		image.getData());
		//}

		glBindTexture(GL_TEXTURE_2D, textureHandle);

		// If texture loaded successfully store its (path,handle)-pair in the map.
		m_textureIdMap[totalTexturePath]=textureHandle;

		check_gl_error();

		return true;
    }
    else
	{
		Logger::GetInstance().Log("Couldn't load Image: %s\n", totalTexturePath.c_str());
		return false;
	}
	return false;
}