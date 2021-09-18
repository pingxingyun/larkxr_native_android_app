//
// Created by fcx@pingixngyun.com on 2019/11/15.
//

#include <log.h>
#include "view.h"
#include "ui/aa_bb.h"
#include "navigation.h"

#define LOG_TAG "view"

namespace {
    const char * DOT             = "textures/ui/circle_active.png";
    const char * ACTIVIVE        = "textures/ui/circle.png";
}

using namespace lark;

float View::VIEW_POSITION_X = 0.0f;
float View::VIEW_POSITION_Y = 0.0F;
float View::VIEW_POSITION_Z = -4.0F;
float View::VIEW_WIDTH      = 6.0F;
float View::VIEW_HEIGHT     = 4.0F;

View::View(Navigation *navigation):
    navigation_(navigation),
    aabb_list_(),
    ray_point_(),
    back_btn_(new BackButton),
    bg_(new ColorBox)
{
    bg_->set_color(component::color(0, 0, 0.1F, 0.15F));
    bg_->set_size(component::size(VIEW_WIDTH, VIEW_HEIGHT));
    bg_->Move(-VIEW_WIDTH / 2, -VIEW_HEIGHT / 2 - 0.5F, - 0.1F);
//    AddChild(bg_);
}

View::~View() {
    // clear aabb.
    ClearAABB();
};

void View::Init() {
    if (back_btn_ && back_btn_->active()) {
        back_btn_->Move(-2.425F, 1.5F, 0.01F);
        // add to aabb
        back_btn_->SetAABBPositon(glm::vec2(-2.42F, 1.5F - back_btn_->GetSize().y / 2.0F));
        PushAABB(back_btn_.get());
        AddChild(back_btn_);
    }

    for (auto & rayDot : ray_dots_) {
        rayDot = std::make_shared<Image>();
        rayDot->SetPath(DOT, true);
        rayDot->set_size(glm::vec2(0.04F, 0.04F));
        // Init far away.
        rayDot->Move(100, 100, 100);
        AddChild(rayDot);
    }
}

void View::HandleInput(Ray * rays, int rayCount) {
    if (Input::GetCurrentRayCastType() != main_ray_cast_) {
        main_ray_cast_ = Input::GetCurrentRayCastType();
        ray_dots_[main_ray_cast_]->SetPath(ACTIVIVE, true);
        for (int i = 0; i < Input::RayCast_Count; i++) {
            if (main_ray_cast_ != i) {
                ray_dots_[i]->SetPath(DOT, true);
            }
        }
    }

    // use curretn transforms.
    Transform world_trans(GetTransforms());

    Plane plane;
    plane.normal = world_trans.Forward();
    plane.dot = world_trans.GetPosition();

//    LOGV("view word position %f %f %f; forward %f %f %f",
//         world_trans.GetPosition()[0], world_trans.GetPosition()[1], world_trans.GetPosition()[2],
//         world_trans.Forward()[0], world_trans.Forward()[1], world_trans.Forward()[2]);

    //    float t;
    //    float dot = utils::Dot(foward, normal);
    //    glm::vec3 f = planeDot - ray.p;
    //    float t =  utils::Dot(f, normal) / dot;
    //    0 -> left 1 -> right 2 -> hmd for now.
    for (int i = 0; i < rayCount; i ++) {
        Ray ray = rays[i];

        float t = (glm::dot(plane.normal, plane.dot) - glm::dot(plane.normal, ray.ori)) / (glm::dot(plane.normal, ray.dir));
        if (t > 0) {
            continue;
        }

        Transform parentTransforms = GetTransforms();
        glm::vec3 parentPosition = parentTransforms.GetPosition();

        glm::vec3 p;
        p.x = ray.ori.x + t * ray.dir.x;
        p.y = ray.ori.y + t * ray.dir.y;
        p.z = ray.ori.z + t * ray.dir.z;

        glm::quat rotation = parentTransforms.GetRotation();
        glm::mat4 wold = glm::translate(glm::mat4(1.0f), p);
        wold = wold * glm::mat4_cast(rotation);
        glm::mat4 local = glm::inverse(parentTransforms.GetTrans()) * wold;
        local = glm::translate(local, glm::vec3(0,0,0.2));
        Transform transform(local);
        glm::vec3 local_position = transform.GetPosition();

//        transform.Translate(p);
        ray_dots_[i]->set_transform(transform);

        //
        ray_point_[i].x = local_position.x;
        ray_point_[i].y = local_position.y;
    }
    for(auto& aabb : aabb_list_) {
        if (aabb->aabb_active()) {
            aabb->HandleInput(ray_point_, rayCount);
        }
    }

    if (navigation_ != nullptr &&
        back_btn_ && back_btn_->active() && back_btn_->picked() &&
            (Input::GetCurrentInputState().triggerShortPressed || Input::GetCurrentInputState().enterShortPressed)) {
        // back to home
        navigation_->SetRouter(Navigation::HOME);
    }
}


void View::Enter() {
}

void View::Leave() {
}

void View::PushAABB(AABB * aabb) {
    if (aabb != nullptr)
        aabb_list_.push_back(aabb);
}

void View::ClearAABB() {
    aabb_list_.clear();
}
