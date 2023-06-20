#pragma once
#include "ObjLoader.h"
#include "OpenGLWrappers.h"
#include <vector>
class Mesh
{
private:
    ObjVertex* vertexArray;
    unsigned int nrOfVertices;
    GLuint* indexArray;
    unsigned int nrOfIndices;

    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    std::vector<size_t> lengths;

    void initVAO()
    {
        //Create VAO
        glCreateVertexArrays(1, &this->VAO);
        glBindVertexArray(this->VAO);

        //GEN VBO AND BIND AND SEND DATA
        glGenBuffers(1, &this->VBO);
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, this->nrOfVertices * sizeof(ObjVertex), this->vertexArray, GL_STATIC_DRAW);

        //GEN EBO AND BIND AND SEND DATA
        if (this->nrOfIndices > 0)
        {
            glGenBuffers(1, &this->EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->nrOfIndices * sizeof(GLuint), this->indexArray, GL_STATIC_DRAW);
        }

        //SET VERTEXATTRIBPOINTERS AND ENABLE (INPUT ASSEMBLY)
        //Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ObjVertex), (GLvoid*)offsetof(ObjVertex, position));
        glEnableVertexAttribArray(0);
        //Normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ObjVertex), (GLvoid*)offsetof(ObjVertex, normal));
        glEnableVertexAttribArray(1);
        //Texcoord
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ObjVertex), (GLvoid*)offsetof(ObjVertex, texcoord));
        glEnableVertexAttribArray(2);

        //BIND VAO 0
        glBindVertexArray(0);
    }

public:
    Mesh(
        ObjVertex* vertexArray,
        const unsigned& nrOfVertices,
        GLuint* indexArray,
        const unsigned& nrOfIndices,
        const std::vector<size_t>& lengths)
    {

        this->nrOfVertices = nrOfVertices;
        this->nrOfIndices = nrOfIndices;

        this->vertexArray = new ObjVertex[this->nrOfVertices];
        for (size_t i = 0; i < nrOfVertices; i++)
        {
            this->vertexArray[i] = vertexArray[i];
        }

        this->indexArray = new GLuint[this->nrOfIndices];
        for (size_t i = 0; i < nrOfIndices; i++)
        {
            this->indexArray[i] = indexArray[i];
        }

        this->initVAO();
        this->lengths = lengths;
    }

    Mesh(const Mesh& obj)
    {

        this->nrOfVertices = obj.nrOfVertices;
        this->nrOfIndices = obj.nrOfIndices;

        this->vertexArray = new ObjVertex[this->nrOfVertices];
        for (size_t i = 0; i < this->nrOfVertices; i++)
        {
            this->vertexArray[i] = obj.vertexArray[i];
        }

        this->indexArray = new GLuint[this->nrOfIndices];
        for (size_t i = 0; i < this->nrOfIndices; i++)
        {
            this->indexArray[i] = obj.indexArray[i];
        }

        this->initVAO();
        this->lengths = obj.lengths;
    }

    ~Mesh()
    {
        glDeleteVertexArrays(1, &this->VAO);
        glDeleteBuffers(1, &this->VBO);

        if (this->nrOfIndices > 0)
        {
            glDeleteBuffers(1, &this->EBO);
        }

        delete[] this->vertexArray;
        delete[] this->indexArray;
    }

    void render(size_t count, unsigned char mode)
    {
        OpenGLManager::checkOpenGLerror();
        size_t from = 0;
        //Bind VAO
        glBindVertexArray(this->VAO);
        //RENDER
        if (this->nrOfIndices == 0) {
            glDrawArraysInstanced(mode, 0, this->nrOfVertices, count);
            /*for (size_t i = 0; i < lengths.size(); i++) {
                glDrawArraysInstanced(mode, from, lengths[i], count);
                from += lengths[i];
            }*/
        }
        else {
            glDrawElementsInstanced(mode, this->nrOfIndices, GL_UNSIGNED_INT, 0, count);
        }
        //Cleanup
        glBindVertexArray(0);
        OpenGLManager::checkOpenGLerror();
    }
};