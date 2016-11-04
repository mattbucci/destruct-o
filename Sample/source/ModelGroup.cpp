/**
 * Copyright (c) 2016, Nathaniel R. Lewis, Anthony Popelar, Matt Bucci, Brian Bamsch
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "stdafx.h"
#include "ModelGroup.h"
#include "lodepng.h"
#include "OS.h"
#include <thread>
#include "Semaphore.h"

// Create an empty model group
ModelGroup::ModelGroup(TextureCache& _textureCache)
    : textureCache(_textureCache)
{

}

// Create a model group and load the models from the manifest
ModelGroup::ModelGroup(const std::string& manifestPath, TextureCache& _textureCache)
    : textureCache(_textureCache)
{
    // Load the compressed data from disk
    std::vector<unsigned char> buffer;
    lodepng::load_file(buffer, manifestPath);

    // If the loading of the file failed, error out
    if(buffer.size() == 0)
    {
        throw new std::runtime_error("Model::Model(const string& manifestPath, TextureCache& _textureCache) - Failed to load: " + manifestPath);
    }

    // Calculate buffer pointers for json decode
    const char *pBegin = (const char *) buffer.data();
    const char *pEnd = (const char *) (buffer.data() + buffer.size());

    // Create a json parser for this file
    Json::Reader reader;
    Json::Value  root;

    // Read the model file
    if(!reader.parse(pBegin, pEnd, root))
    {
        throw new std::runtime_error("Model::Model(const string& manifestPath, TextureCache& _textureCache) - Parsing failed for: " + manifestPath);
    }

    // The root must be an array
    if(root == Json::Value::null || !root.isArray())
    {
        throw new std::runtime_error("Model::Model(const string& manifestPath, TextureCache& _textureCache) - Manifest root is not an array");
    }

    // Get start time to measure the load time of the models
	double start = OS::Now();

    // Get an iterator to the current Json value
    Json::Value::iterator modelIterator = root.begin();

    // Create a mutex to lock the critical sections of the model loader worker threads
	mutex * modelIteratorMutex = new mutex();
	semaphore * deadThreads = new semaphore();

	static const int workerThreads = 4;

	// Launch four worker threads
	for (int i = 0; i < workerThreads; i++)
    {
		thread t([&,modelIteratorMutex]()
        {
			while (1) {
				//Critical section
				modelIteratorMutex->lock();

				//Check if all models are loaded
				if (modelIterator == root.end())
                {
					modelIteratorMutex->unlock();
					deadThreads->increase();
					return;
				}

				// Otherwise, get a reference to the current model to load
                const Json::Value& model = *modelIterator;
                modelIterator++;
				modelIteratorMutex->unlock();

				// Get the name of the model
				std::string name = (model)["name"].asString();

                // Create a ModelInstance from this entry
                double s = OS::Now();
                ModelInstance *instance = ModelInstance::LoadManifestEntry(model, textureCache);
                double f = OS::Now() - s;
                cout << "Loaded: " << name << " (in " << f << " seconds)" << endl;

                // Insert this into the model group
                modelGroupMutex.lock();
                models[name] = instance;
                modelGroupMutex.unlock();
			}
		});
		t.detach();
	}

	//Decrease four times, once for each thread
	for (int i = 0; i < workerThreads; i++)
    {
		deadThreads->decrease();
    }

	cout << "All meshes loaded: " << OS::Now()-start << "s \n";

	delete modelIteratorMutex;
	delete deadThreads;
}

// Create a new model instance from a model in the group
ModelInstance* ModelGroup::NewInstance(const std::string modelName)
{
    // Lookup the model name
    std::map<std::string, ModelInstance *>::iterator model = models.find(modelName);

    // If the model loaded successfully
    if(model != models.end())
    {
        ModelInstance *instance = new ModelInstance(*(model->second));
        return instance;
    }

    // Otherwise fail out
    return NULL;
}

// Get iterators
ModelGroup::const_iterator ModelGroup::begin() const
{
    return models.begin();
}

ModelGroup::const_iterator ModelGroup::end() const
{
    return models.begin();
}

ModelGroup::size_type ModelGroup::size() const
{
    return models.size();
}
