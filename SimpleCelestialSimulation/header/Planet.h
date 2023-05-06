#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include <iostream>
using namespace std;

class Planet
{
public:
    string name;
    ///����-------------------------------------------
    glm::vec3 scaling;      //ģ�����ű���
    
    glm::vec3 position;     //λ��

    float rotateAngle_s;    //��ת�Ƕ�
    glm::vec3 rotateAxis_s; //��ת��

    float rotateAngle_r;    //��ת�Ƕ�

    float rotateSpeed_s;    //��ת�ٶ�
    float rotateSpeed_r;    //��ת�ٶ�
  
    //�빫תĿ��֮��ľ���
    float disToTar;

    //mvp����
    glm::mat4 modelMat;
    glm::mat4 viewMat;
    glm::mat4 projectionMat;

    ///����-------------------------------------------

    Planet(string _name,const string& modelPath,
        float _disToTar, 
        float _rotateSpeed_s, glm::vec3 _rotateAxis_s,
        float _rotateSpeed_r, 
        glm::vec3 _scaling);

    //����
    void Draw(Shader& _shader);
    //����mvp
    void SetMVP(Shader& _shader);
    //����v_p����
    void Update_V_P(Camera& _camera, float _fov);
    //����m����
    void Update_M();
    //��ת
    void Rotate(float _dt);
    //λ��
    void SetPosition(glm::vec3 _position);
    //��Ŀ����ת
    void RotateAround(glm::vec3 _targetPos, float _dt);
    //������ת��
    void UpdataRotateAxis_s();
private:

    //ģ������
    Model model;

    int sgn;    //����ʹ�õ�����
    int theta;
    int yuShu;
    float alpha;
};



Planet::Planet(string _name, const string& modelPath,
    float _disToTar = 0.0f, 
    float _rotateSpeed_s = 0.0f, glm::vec3 _rotateAxis_s = glm::vec3(0.0f, 1.0f, 0.0f),
    float _rotateSpeed_r = 0.0f,
    glm::vec3 _scaling = glm::vec3(1.0f, 1.0f, 1.0f))
        :name(_name),disToTar(_disToTar), rotateSpeed_s(_rotateSpeed_s), rotateAxis_s(_rotateAxis_s),rotateSpeed_r(_rotateSpeed_r),
        scaling(_scaling)
{
    //����ģ��
    model.Init(modelPath);
    //��ʼ����������
    rotateAngle_r = rotateAngle_s = 0.0f;
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    viewMat = projectionMat = glm::mat4(1.0f);
    modelMat = glm::mat4(1.0f);

    sgn = 1;
    theta = 0;
    yuShu = -1;
    alpha = 0.0f;
}

void Planet::Draw(Shader& _shader)
{
    model.Draw(_shader);
}


void Planet::Update_V_P(Camera& _camera, float _fov)
{
    viewMat = _camera.GetViewMatrix();
    projectionMat = glm::perspective(glm::radians(_camera.Zoom), _fov, 0.1f, 100.0f);
}

void Planet::SetPosition(glm::vec3 _position)
{
    position = _position;
}

void Planet::SetMVP(Shader& _shader)
{
    _shader.use();
    _shader.setMat4("view", viewMat);
    _shader.setMat4("projection", projectionMat);
    _shader.setMat4("model", modelMat);
}

//��ת
void Planet::Rotate(float _dt)
{
    rotateAngle_s += rotateSpeed_s * _dt;
}

//��ת
void Planet::RotateAround(const glm::vec3 _targetPos, float _dt)
{
    //������Ŀ��ľ��루ʡ���ˣ���ʱ�ù̶��뾶��
    rotateAngle_r -= rotateSpeed_r * _dt;
    
    glm::vec3 newPos;
    if (name == "moon") //��������ת�׵��ļн�
    {
        int temp = (int)-rotateAngle_r % 180;

        if (yuShu != temp)  //�����ǶȲŸ���
        {
            yuShu = temp;
            if (sgn == 1)
            {
                theta = yuShu;
                if (theta == 179)sgn = -1;
            }
            else
            {
                theta = 180 - yuShu;
                if (theta == 1)sgn = 1;
            }
            float t = theta / 180.0f;
            alpha = 23.5f * t + -23.5f * (1 - t);   //���Բ�ֵ
        }

        newPos.x = _targetPos.x + disToTar * glm::cos(glm::radians(rotateAngle_r)) * glm::cos(glm::radians(alpha));
        newPos.y = glm::sin(glm::radians(alpha));
        newPos.z = _targetPos.z + disToTar * glm::sin(glm::radians(rotateAngle_r)) * glm::cos(glm::radians(alpha));
    }
    else
    {
        newPos.x = _targetPos.x + disToTar * glm::cos(glm::radians(rotateAngle_r));
        newPos.y = position.y;      
        newPos.z = _targetPos.z + disToTar * glm::sin(glm::radians(rotateAngle_r));
    }
    SetPosition(newPos);
}

void Planet::Update_M()
{
    modelMat = glm::mat4(1.0f);
    //s��r��t������
    //t
    modelMat = glm::translate(modelMat, position);  
    //s
    modelMat = glm::scale(modelMat, scaling);
    //r
    modelMat = glm::rotate(modelMat, rotateAngle_s, rotateAxis_s);
}

void Planet::UpdataRotateAxis_s()
{
    glm::vec3 newAxis;
    
    newAxis.x = 0.397f * glm::cos(glm::radians(rotateAngle_r));
    newAxis.y = 0.918f;
    newAxis.z = 0.397f * glm::sin(glm::radians(rotateAngle_r));
    rotateAxis_s = glm::normalize(newAxis);
}