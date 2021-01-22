//
// LICENSE:
//
// Copyright (c) 2016 -- 2021 Fabio Pellacini
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#include <yocto/yocto_commonio.h>
#include <yocto/yocto_image.h>
#include <yocto/yocto_json.h>
#include <yocto/yocto_math.h>
#include <yocto/yocto_sceneio.h>
#include <yocto/yocto_trace.h>
#if YOCTO_OPENGL == 1
#include <yocto_gui/yocto_imageviewer.h>
#endif
using namespace yocto;

// Construct a scene from io
void init_scene(trace_scene* scene, sceneio_scene* ioscene,
    trace_camera*& camera, sceneio_camera* iocamera,
    progress_callback progress_cb = {}) {
  // handle progress
  auto progress = vec2i{
      0, (int)ioscene->cameras.size() + (int)ioscene->environments.size() +
             (int)ioscene->materials.size() + (int)ioscene->textures.size() +
             (int)ioscene->shapes.size() + (int)ioscene->instances.size()};

  auto camera_map     = unordered_map<sceneio_camera*, trace_camera*>{};
  camera_map[nullptr] = nullptr;
  for (auto iocamera : ioscene->cameras) {
    if (progress_cb)
      progress_cb("converting cameras", progress.x++, progress.y);
    auto camera          = add_camera(scene);
    camera->frame        = iocamera->frame;
    camera->lens         = iocamera->lens;
    camera->aspect       = iocamera->aspect;
    camera->film         = iocamera->film;
    camera->orthographic = iocamera->orthographic;
    camera->aperture     = iocamera->aperture;
    camera->focus        = iocamera->focus;
    camera_map[iocamera] = camera;
  }

  auto texture_map     = unordered_map<sceneio_texture*, trace_texture*>{};
  texture_map[nullptr] = nullptr;
  for (auto iotexture : ioscene->textures) {
    if (progress_cb)
      progress_cb("converting textures", progress.x++, progress.y);
    auto texture           = add_texture(scene);
    texture->hdr           = iotexture->hdr;
    texture->ldr           = iotexture->ldr;
    texture_map[iotexture] = texture;
  }

  auto material_map     = unordered_map<sceneio_material*, trace_material*>{};
  material_map[nullptr] = nullptr;
  for (auto iomaterial : ioscene->materials) {
    if (progress_cb)
      progress_cb("converting materials", progress.x++, progress.y);
    auto material              = add_material(scene);
    material->emission         = iomaterial->emission;
    material->color            = iomaterial->color;
    material->specular         = iomaterial->specular;
    material->roughness        = iomaterial->roughness;
    material->metallic         = iomaterial->metallic;
    material->ior              = iomaterial->ior;
    material->spectint         = iomaterial->spectint;
    material->coat             = iomaterial->coat;
    material->transmission     = iomaterial->transmission;
    material->translucency     = iomaterial->translucency;
    material->scattering       = iomaterial->scattering;
    material->scanisotropy     = iomaterial->scanisotropy;
    material->trdepth          = iomaterial->trdepth;
    material->opacity          = iomaterial->opacity;
    material->thin             = iomaterial->thin;
    material->emission_tex     = texture_map.at(iomaterial->emission_tex);
    material->color_tex        = texture_map.at(iomaterial->color_tex);
    material->specular_tex     = texture_map.at(iomaterial->specular_tex);
    material->metallic_tex     = texture_map.at(iomaterial->metallic_tex);
    material->roughness_tex    = texture_map.at(iomaterial->roughness_tex);
    material->transmission_tex = texture_map.at(iomaterial->transmission_tex);
    material->translucency_tex = texture_map.at(iomaterial->translucency_tex);
    material->spectint_tex     = texture_map.at(iomaterial->spectint_tex);
    material->scattering_tex   = texture_map.at(iomaterial->scattering_tex);
    material->coat_tex         = texture_map.at(iomaterial->coat_tex);
    material->opacity_tex      = texture_map.at(iomaterial->opacity_tex);
    material->normal_tex       = texture_map.at(iomaterial->normal_tex);
    material_map[iomaterial]   = material;
  }

  auto shape_map     = unordered_map<sceneio_shape*, trace_shape*>{};
  shape_map[nullptr] = nullptr;
  for (auto ioshape : ioscene->shapes) {
    if (progress_cb) progress_cb("converting shapes", progress.x++, progress.y);
    auto shape              = add_shape(scene);
    shape->points           = ioshape->points;
    shape->lines            = ioshape->lines;
    shape->triangles        = ioshape->triangles;
    shape->quads            = ioshape->quads;
    shape->quadspos         = ioshape->quadspos;
    shape->quadsnorm        = ioshape->quadsnorm;
    shape->quadstexcoord    = ioshape->quadstexcoord;
    shape->positions        = ioshape->positions;
    shape->normals          = ioshape->normals;
    shape->texcoords        = ioshape->texcoords;
    shape->colors           = ioshape->colors;
    shape->radius           = ioshape->radius;
    shape->tangents         = ioshape->tangents;
    shape->subdivisions     = ioshape->subdivisions;
    shape->catmullclark     = ioshape->catmullclark;
    shape->smooth           = ioshape->smooth;
    shape->displacement     = ioshape->displacement;
    shape->displacement_tex = texture_map.at(ioshape->displacement_tex);
    shape_map[ioshape]      = shape;
  }

  for (auto ioinstance : ioscene->instances) {
    if (progress_cb)
      progress_cb("converting instances", progress.x++, progress.y);
    auto instance      = add_instance(scene);
    instance->frame    = ioinstance->frame;
    instance->shape    = shape_map.at(ioinstance->shape);
    instance->material = material_map.at(ioinstance->material);
  }

  for (auto ioenvironment : ioscene->environments) {
    if (progress_cb)
      progress_cb("converting environments", progress.x++, progress.y);
    auto environment          = add_environment(scene);
    environment->frame        = ioenvironment->frame;
    environment->emission     = ioenvironment->emission;
    environment->emission_tex = texture_map.at(ioenvironment->emission_tex);
  }

  // done
  if (progress_cb) progress_cb("converting done", progress.x++, progress.y);

  // get camera
  camera = camera_map.at(iocamera);
}


// jsonファイルの数を入れ、その下のview_paramsのscenes配列にjsonのパスを入れる
// set scene json num
static const int SCENE_NUM = 3;

// convert params
struct view_params : trace_params {
  string output    = "out.png";

  // scene json list
  string scenes[SCENE_NUM] = {
     "../../../tests/environments1/environments1.json",
     "../../../tests/zizo1/zizou1.json",
     "../../../tests/materials1/materials1.json",
  };
  string camera    = "";
  bool   addsky    = false;
  bool   savebatch = false;
};

// Json IO
void serialize_value(json_mode mode, json_value& json, view_params& value,
    const string& description) {
  serialize_object(mode, json, value, description);
  for (int i=0; i<SCENE_NUM; i++)
	serialize_property(mode, json, value.scenes[i], "scenes"+i, "Scene filename."+i, true);
  //serialize_property(mode, json, value.scenes, "scenes", "Scene filename.", true);
  serialize_property(mode, json, value.output, "output", "Output filename.");
  serialize_property(mode, json, value.camera, "camera", "Camera name.");
  serialize_property(mode, json, value.addsky, "addsky", "Add sky.");
  serialize_property(mode, json, value.savebatch, "savebatch", "Save batch.");
  serialize_value(mode, json, (trace_params&)value, description);
  serialize_clipositionals(mode, json, {"scene"});
  serialize_clialternates(mode, json,
      {{"samples", "s"}, {"bounces", "b"}, {"output", "o"}, {"tracer", "t"}});
}

#ifndef YOCTO_OPENGL

// convert images
int run_view(const view_params& params, const imageview_state& viewer) {
  return print_fatal("Opengl not compiled");
}

#else

// draw brush for canvas buffer
void draw_brush(trace_state* state, imageview_state* viewer, float threshold) {
  parallel_for(state->brush.w, state->brush.h, [&](int i, int j) {
    int _i = i + state->brush.x;
    int _j = j + state->brush.y;

    if (_i < 0 || _i > state->render.width() - 1 || _j < 0 ||
        _j > state->render.height() - 1)
      return;
    vec2i _ij = {_i, _j};

    float rnd = rand1f(state->rngs[{1, 1}]);

    vec2f a = {float(_i - state->brush.w / 2), float(_j - state->brush.h / 2)};
    vec2f b = {float(state->brush.x), float(state->brush.y)};

    

    float dist  = distance(a, b);
    float value = clamp(1 - dist / state->brush.w * 2, 0.0, 1.0);
    // printf("%f \n", value);
    if (value - rnd < threshold) return;
    state->canvas[_ij] = {1, 0, 0, 1};
  });

  // set_image(viewer, "canvas", state->canvas);
}


// interactive render
int run_view(const view_params& params) {

#pragma region Setting scene

  auto viewer_guard = make_imageview("yimage");
  auto viewer       = viewer_guard.get();

  // scene loading
  std::unique_ptr<trace_scene[]> scene_guards = std::make_unique<trace_scene[]>(SCENE_NUM);
  std::unique_ptr<trace_bvh[]> bvh_guards = std::make_unique<trace_bvh[]>(SCENE_NUM);
  std::unique_ptr<trace_lights[]> lights_guards = std::make_unique<trace_lights[]>(SCENE_NUM);
  
  trace_scene*  scene_arr[SCENE_NUM];
  trace_camera* camera_arr[SCENE_NUM];
  trace_bvh*    bvh_arr[SCENE_NUM];
  trace_lights* lights_arr[SCENE_NUM];

  for (int i = 0; i < SCENE_NUM; i++) {
    // scene loading
    auto ioscene_guard = std::make_unique<sceneio_scene>();
   
    auto ioscene       = ioscene_guard.get();
    auto ioerror       = string{};
    if (!load_scene(params.scenes[i], ioscene, ioerror, print_progress))
      return print_fatal(ioerror);

    // add sky
    if (params.addsky) add_sky(ioscene);

    // get camera
    auto iocamera = get_camera(ioscene, params.camera);

    // scene conversion
    scene_arr[i]  = &scene_guards.get()[i];
    camera_arr[i]       = nullptr;
    init_scene(scene_arr[i], ioscene, camera_arr[i], iocamera);

    // cleanup
    ioscene_guard.reset();

    // tesselation
    tesselate_shapes(scene_arr[i], print_progress);

    // build bvh
    bvh_arr[i]       = &bvh_guards.get()[i];
    init_bvh(bvh_arr[i], scene_arr[i], params, print_progress);

    // init renderer
    lights_arr[i] = &lights_guards.get()[i];
    init_lights(lights_arr[i], scene_arr[i], params, print_progress);

    // fix renderer type if no lights
    if (lights_arr[i]->lights.empty() && is_sampler_lit(params)) {
      print_info("no lights presents, image will be black");
    }
  }

  int           current_scene = 0;
  trace_scene*  scene             = scene_arr[current_scene];
  trace_camera* camera            = camera_arr[current_scene];
  trace_bvh*    bvh               = bvh_arr[current_scene];
  trace_lights* lights            = lights_arr[current_scene];



#pragma endregion

  // init state
  auto state_guard = std::make_unique<trace_state>();
  auto state       = state_guard.get();

  // render start
  // reset canvas
  trace_start(
      state, scene, camera, bvh, lights, params,
      [viewer](const string& message, int sample, int nsamples) {
        set_param(viewer, "render", "sample", to_json(sample),
            to_schema(sample, "Current sample"));
        print_progress(message, sample, nsamples);
      },
      [viewer](const image<vec4f>& render, const image<vec4f>& canvas,
          int current, int total) {
        set_image(viewer, "render", render);
        // set_image(viewer, "canvas", canvas);
      });

  // show rendering params
  set_params(
      viewer, "render", to_json(params), to_schema(params, "Render params"));


  // set_callback(viewer, )

  // set callback
  // Viewerの変更のコールバック
  set_callback(viewer, [state, &scene, &camera, &bvh, &lights, viewer, &params](
                           const string& name, const json_value& uiparams,
                           const gui_input& input) {
    if (name != "render" && name != "canvas") return;

    if (!uiparams.is_null()) {
      trace_stop(state);

      (view_params&)params = from_json<view_params>(uiparams);
      // show rendering params
      set_params(viewer, "render", to_json(params),
          to_schema(params, "Render params"));

      auto pprms = params;
      pprms.resolution /= params.pratio;
      pprms.samples = 1;
      auto preview  = trace_image(scene, camera, bvh, lights, pprms);

    } else if ((input.mouse_left || input.mouse_right) &&
               input.mouse_pos != input.mouse_last) {

      printf(
          "mouse_pos__ x: %f, y: %f \n", input.mouse_pos.x, input.mouse_pos.y);

      int barHeight = int((input.window_size.y - state->render.height()) / 2);

      state->brush.w     = 400;
      state->brush.h     = 400;
      
        float window_ratio = float(input.window_size.x) /
            float(input.window_size.y);

        vec2f npos = {float(input.mouse_pos.x) / float(input.window_size.x),
            float(input.mouse_pos.y) / float(input.window_size.y)};

        printf("normalized_pos__ x: %f, y: %f \n", npos.x, npos.y);

        state->brush.x = int( npos.x * state->render.width() - (state->brush.w / 2));
        state->brush.y = int( npos.y * state->render.height() -(state->brush.h / 2));

          
          
          draw_brush(state, viewer, 0.5);
          

          trace_step(
               state, scene, camera, bvh, lights, params,
               [viewer](const string& message, int sample, int nsamples) {
        set_param(viewer, "render", "sample", to_json(sample),
            to_schema(sample, "Current sample"));
        print_progress(message, sample, nsamples);
               },
              [viewer](const image<vec4f>& render, const image<vec4f>& canvas,
                  int current,
                  int total) {
        set_image(viewer, "render", render);
        // set_image(viewer, "canvas", canvas);
               });
    }


  });

  set_callback(viewer,[state, &scene, &camera, &bvh, &lights, scene_arr, camera_arr, bvh_arr, lights_arr, viewer, &current_scene, &params](const int& key, const bool& pressed, const gui_input& input){ printf("key pressed   keyid_%d pressed_%d \n", key, pressed);

      printf("key pressed   keyid_%d pressed_%d \n", key, pressed);
      // ここ以下をキーインプットのif文の中にまるごと入れる
      trace_stop(state);
        
      current_scene = (current_scene + 1) % SCENE_NUM;
      
      scene  = scene_arr[current_scene];
      camera = camera_arr[current_scene];
      bvh    = bvh_arr[current_scene];
      lights = lights_arr[current_scene];


      trace_start(
          state, scene, camera, bvh, lights, params,
          [viewer](const string& message, int sample, int nsamples) {
            set_param(viewer, "render", "sample", to_json(sample),
                to_schema(sample, "Current sample"));
            print_progress(message, sample, nsamples);
          },
          [viewer](const image<vec4f>& render, const image<vec4f>& canvas,
              int current, int total) {
            set_image(viewer, "render", render);
            // set_image(viewer, "canvas", canvas);
          });


      if(key == 321 && pressed == false){ //numpad 1
      };

  });

  // run view
  bool bProduction = true;  // ここをtrueにするとフルスクリーン
  if (bProduction)
    yocto::run_view(viewer, {2160, 3840}, false, true);
  else
    yocto::run_view(viewer, {1080, 1920}, true, false);

  // stop
  trace_stop(state);

  // done
  return 0;
}

#endif

struct app_params {
  string command = "view";
  //  render_params render  = {};
  view_params view = {};
};

// Json IO
void serialize_value(json_mode mode, json_value& json, app_params& value,
    const string& description) {
  serialize_object(mode, json, value, description);
  serialize_command(mode, json, value.command, "command", "Command.");
  //  serialize_property(mode, json, value.render, "render", "Render offline.");
  serialize_property(mode, json, value.view, "view", "Render interactively.");
}


int main(int argc, const char* argv[]) {
  // parse cli
  auto params = app_params{};

  params.view.samples    = 64;
  params.view.resolution = 3840;
  params.view.pratio     = 2;
  //params.view.scene      = "../../../tests/zizo1/zizou1.json";

  // parse_cli(params, "Render images from scenes", argc, argv);

  // dispatch commands

  // open viewer
  

  return run_view(params.view);
}
