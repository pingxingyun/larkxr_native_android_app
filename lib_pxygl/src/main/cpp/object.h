//
// Created by fcx@pingxingyun.com on 2019/11/7.
//

#ifndef MY_APPLICATION_OBJECT_H
#define MY_APPLICATION_OBJECT_H

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#ifdef __ANDROID__
#include <android/asset_manager.h>
#include "asset_file.h"
#endif
#include "shader.h"
#include "transform.h"

namespace lark {
class VertexArrayObject;
class Texture;
class CLOUDLARK_PXYGL_API Object {
private:
    static uint64_t objcet_count_;
public:
    enum Eye {
        EYE_LEFT,
        EYE_RIGHT
    };

    Object();
    virtual ~Object();

    static inline bool HasGlExtension(const char * ext_name) {
        // It's not effencient but only use at initial.
        const char *v = (const char *) glGetString(GL_EXTENSIONS);
        std::string extensions = v;
        return !extensions.empty() &&
               extensions.find(ext_name) != std::string::npos;
    }
    virtual inline void set_enable(bool enable) { enable_ = enable;  };

    inline bool isEnabled() const {
        return enable_;
    }

    bool AreEnabled() const;

    inline bool has_error() const {
        return has_error_;
    }

    bool HasGLError() const;

    inline void set_parent(Object * parent) {
        parent_ = parent;
    }

    inline Object * parent() {
        return parent_;
    }

    inline Object* GetParentRoot() {
        if (parent_ != NULL)
            return GetParentRoot();
        return this;
    }

    inline Object* Move(float x, float y, float z) {
        transform_.Translate(x, y, z);
        return this;
    };

    inline Object* Move(const glm::vec3 & position) {
        transform_.Translate(position);
        return this;
    };

    Object* Rotate(float radians, const glm::vec3 & rotate) {
        transform_.Rotate(radians, rotate);
        return this;
    };

    Object* Scale(float scale) {
        transform_.Sacle(scale);
        return this;
    };
    inline Transform& transform() { return transform_; }
    inline void set_transform(const glm::mat4& transform) { transform_ = transform; }
    inline void set_transform(const Transform& transform) { transform_ = transform; }

    inline glm::quat GetRotation() { return transform_.GetRotation(); }

    glm::mat4 GetTransforms() const;

    glm::mat3 MakeNormalMatrix(const glm::mat4& view) const;

    // id
    inline uint64_t id() { return id_; }

    inline void set_active(bool active) { active_ = active; }
    inline bool active() { return active_; }

    // update every frame
    virtual void Update();
    // draw
    virtual void Draw(Eye eye, const glm::mat4& projection, const glm::mat4& view);
    virtual void DrawMultiview(const glm::mat4& projection, const glm::mat4& view);

    // manange child.
    void AddChild(std::shared_ptr<Object>&& child);
    void RemoveChild(std::shared_ptr<Object>&& child);
    void ClearChild();
protected:
    virtual void InitVao(const void* vertices, int verticesSize, const void* indices, int indicesSize);
#ifdef __ANDROID__
    void LoadShaderFromAsset(AAssetManager* assetManager, const char* vfile, const char* ffile);
    void LoadMultiviewShaderFromAsset(AAssetManager* assetManager, const char* vfile, const char* ffile);
#else
    void LoadShaderFromAsset(const char* vfile, const char* ffile);
    void LoadMultiviewShaderFromAsset(const char* vfile, const char* ffile);
#endif

    void LoadShader(const char* vfile, const char* ffile, const char* vstr, const char* fstr);
    void LoadMultviewShader(const char* vfile, const char* ffile, const char* vstr, const char* fstr);

    bool active_;

    const char * name_;
    Object* parent_;
    Transform transform_;
    std::shared_ptr<Shader> shader_;
    std::shared_ptr<Shader> multiview_shader_;
    std::shared_ptr<VertexArrayObject> vao_;
    std::shared_ptr<Texture> texture_;
    bool enable_;
    bool has_error_;
    uint64_t id_;

    // objects.
    std::vector<std::shared_ptr<Object>> children_objects_;
};
}

#endif //MY_APPLICATION_OBJECT_H
