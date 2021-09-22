//
// Created by fcx@pingixngyun.com on 2019/11/15.
//

#ifndef CLOUDLARK_OCULUS_DEMO_VIEW_H
#define CLOUDLARK_OCULUS_DEMO_VIEW_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <object.h>
#include <list>
#include "input.h"
//#include <scene/ray_sphereIntersection.h>
//#include "app_nav_btns.h"
#include "component/button.h"
#include "ui/aa_bb.h"

class Navigation;
class View: public  lark::Object {
public:
    static float VIEW_POSITION_X;
    static float VIEW_POSITION_Y;
    static float VIEW_POSITION_Z;
    static float VIEW_WIDTH;
    static float VIEW_HEIGHT;

    View(Navigation *navigation);
    ~View();
    virtual void HandleInput(lark::Ray * rays, int rayCount);
    // switch to current page.
    virtual void Enter();
    // leave current page.
    virtual void Leave();
protected:
    virtual void Init();

    void PushAABB(AABB * aabb);
    // clear all aabb;
    void ClearAABB();
    // TODO add aaabb remove.

    Navigation                   *navigation_;
    std::shared_ptr<BackButton>  back_btn_;
    std::shared_ptr<Image>       left_ray_dot_;
    std::shared_ptr<Image>       right_ray_dot_;
    std::shared_ptr<ColorBox>    bg_;

    // 0 -> left ray dot; 1 -> right ray dot;
    std::shared_ptr<Image>       ray_dots_[Input::RayCast_Count];
    // 0 -> left ray dot; 1 -> right ray dot;
    glm::vec2                    ray_point_[Input::RayCast_Count];
    // aabb check list.
    std::list<AABB*>             aabb_list_;
    Input::RayCastType           main_ray_cast_ = Input::RayCast_left;
};


#endif //CLOUDLARK_OCULUS_DEMO_VIEW_H
