/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#include "code_rampcolor_byte.h"




namespace csg {

	void copy_v3_v3(uint32_t* r, const float a[3])
	{
        char* pixels = (char*)r;
		pixels[2] = a[0]*255;
		pixels[1] = a[1]*255;
		pixels[0] = a[2]*255;
	}

	int gpu_material_ramp_texture_row_set(const std::shared_ptr<cse::SharedState> shareState, int size, uint32_t* pixels, int layer) {
		if (layer >= CM_TABLE) {
			//越界了,每帧最多使用的band
            assert(false);
		}
		else {
			shareState->UpdateLayerRampBuffer(pixels,layer);
		}
        return layer+1;
	}

	int GPU_color_band(const std::shared_ptr<cse::SharedState> shareState, int size, uint32_t* pixels, int layer) {
        return gpu_material_ramp_texture_row_set(shareState, size, pixels, layer);
//		free(pixels);
//		return texId;
	}

	void BKE_curvemapping_table_RGBA(const Curve* cumap_x,const Curve* cumap_y,const Curve* cumap_z,const Curve* cumap_w, uint32_t** array, int* size)
	{
		int a;

		*size = MAX_COLOR_BAND;
		*array = (uint32_t*)malloc(sizeof(uint32_t) * (*size));

		//直接在curve里面缓存,不用每次算
		auto cached_x_curve = cumap_x->eval_curve<MAX_COLOR_BAND>();
		auto cached_y_curve =  cumap_y->eval_curve<MAX_COLOR_BAND>();
		auto cached_z_curve =  cumap_x->eval_curve<MAX_COLOR_BAND>();
        std::array<float, MAX_COLOR_BAND> cached_w_curve;
        if(cumap_w != nullptr){
            cached_w_curve = cumap_w->eval_curve<MAX_COLOR_BAND>();
        }
        char* pixels = (char*)*array;
		for (a = 0; a < *size; a++) {
			//if (cumap->cm[0].table)
			{
				(pixels)[a*4+2] = cached_x_curve[a]*255;
			}
			//if (cumap->cm[1].table)
			{
				(pixels)[a*4+1] = cached_y_curve[a]*255;
			}
			//if (cumap->cm[2].table)
			{
				(pixels)[a*4] = cached_z_curve[a]*255;
			}
			if (cumap_w != nullptr)
			{
				(pixels)[a * 4 + 3] = cached_w_curve[a]*255;
			}
		}
	}

	/// <summary>
	/// 将curve数据转为ramptex
	/// </summary>
	/// <param name="mat"></param>
	/// <param name="cumap"></param>
	/// <param name="accessLayer"></param>
	/// <returns></returns>
    int SetVectorCurveSlotValue(const std::shared_ptr<cse::SharedState> shareState, const VectorCurveSlotValue* cumap,int accessLayer,csc::Float3* range,csc::Float4* extx,csc::Float4* exty,csc::Float4* extz) {
        uint32_t* array;
		int size;

		BKE_curvemapping_table_RGBA(cumap->get_x_ptr(),cumap->get_y_ptr(),cumap->get_z_ptr(),nullptr, &array, &size);

		accessLayer = GPU_color_band(shareState, size, array, accessLayer);
        

        if(range != nullptr){
            csc::Float4* ext_xyz[3];
            ext_xyz[0] =extx;
            ext_xyz[1] =exty;
            ext_xyz[2] =extz;
            float* range_xyz = (float*)range;
            const Curve* cms[4];
            cms[0] = cumap->get_x_ptr();
            cms[1] = cumap->get_y_ptr();
            cms[2] = cumap->get_z_ptr();

            for (int a = 0; a < 3; a++) {
                const Curve* cm = cms[a];
                ext_xyz[a]->x = cm->min().x;
                ext_xyz[a]->z = cm->max().x;
                range_xyz[a] = 1.0f / std::max(1e-8f, cm->max().x - cm->min().x);
                /* Compute extrapolation gradients. */
                //if ((cumap->flag & CUMA_EXTEND_EXTRAPOLATE) != 0) {
                //	ext_xyz[a][1] = (cm->ext_in[0] != 0.0f) ? (cm->ext_in[1] / (cm->ext_in[0] * range_xyz[a])) :
                //		1e8f;
                //	ext_xyz[a][3] = (cm->ext_out[0] != 0.0f) ?
                //		(cm->ext_out[1] / (cm->ext_out[0] * range_xyz[a])) :
                //		1e8f;
                //}
                //else
                {
                    ext_xyz[a]->y = 0.0f;
                    ext_xyz[a]->w = 0.0f;
                }
            }
        }
        return accessLayer;
	}


    int SetRGBCurveSlotValue(const std::shared_ptr<cse::SharedState> shareState,const RGBCurveSlotValue* cumap,int accessLayer,csc::Float4* range,csc::Float4* extx,csc::Float4* exty,csc::Float4* extz,csc::Float4* extw) {
        uint32_t* array;
        int size;

        BKE_curvemapping_table_RGBA(cumap->get_x_ptr(),cumap->get_y_ptr(),cumap->get_z_ptr(),cumap->get_all_ptr(), &array, &size);

        accessLayer = GPU_color_band(shareState, size, array, accessLayer);
        

        if(range != nullptr){
            csc::Float4* ext_xyzw[4];
            ext_xyzw[0] =extx;
            ext_xyzw[1] =exty;
            ext_xyzw[2] =extz;
            ext_xyzw[3] =extw;
            float* range_xyzw = (float*)range;
            const Curve* cms[4];
            cms[0] = cumap->get_x_ptr();
            cms[1] = cumap->get_y_ptr();
            cms[2] = cumap->get_z_ptr();
            cms[3] = cumap->get_all_ptr();

            for (int a = 0; a < 4; a++) {
                const Curve* cm = cms[a];
                ext_xyzw[a]->x = cm->min().x;
                ext_xyzw[a]->z = cm->max().x;
                range_xyzw[a] = 1.0f / std::max(1e-8f, cm->max().x - cm->min().x);
                /* Compute extrapolation gradients. */
                //if ((cumap->flag & CUMA_EXTEND_EXTRAPOLATE) != 0) {
                //    ext_xyz[a][1] = (cm->ext_in[0] != 0.0f) ? (cm->ext_in[1] / (cm->ext_in[0] * range_xyz[a])) :
                //        1e8f;
                //    ext_xyz[a][3] = (cm->ext_out[0] != 0.0f) ?
                //        (cm->ext_out[1] / (cm->ext_out[0] * range_xyz[a])) :
                //        1e8f;
                //}
                //else
                {
                    ext_xyzw[a]->y = 0.0f;
                    ext_xyzw[a]->w = 0.0f;
                }
            }
        }
        return accessLayer;
    }


	void BKE_colorband_evaluate_table_rgba(const ColorRampSlotValue* coba, uint32_t** array, int* size)
	{
		int a;

		*size = MAX_COLOR_BAND;
		*array = (uint32_t*)malloc(sizeof(uint32_t) * (*size));

		//直接在curve里面缓存,不用每次算
		
        char* pixels = (char*)*array;
		for (a = 0; a < *size; a++) {
			const float fraction{ static_cast<float>(a) / static_cast<float>(MAX_COLOR_BAND) };
			auto cached_curve = coba->get().eval(fraction);
            (pixels)[a * 4] = cached_curve.z*255;
			(pixels)[a * 4 +1] = cached_curve.y*255;
			(pixels)[a * 4 +2] = cached_curve.x*255;
			(pixels)[a * 4 +3] = cached_curve.w*255;
		}
	}

	int SetColorRampSlotValue(const std::shared_ptr<cse::SharedState> shareState,const ColorRampSlotValue* cumap,int accessLayer) {
        uint32_t* array;
		int size;

		BKE_colorband_evaluate_table_rgba(cumap, &array, &size);

		accessLayer = GPU_color_band(shareState, size, array, accessLayer);

        
        return accessLayer;
	}





	float cie_colour_match[81][3] = {
	{0.0014f, 0.0000f, 0.0065f}, {0.0022f, 0.0001f, 0.0105f}, {0.0042f, 0.0001f, 0.0201f},
	{0.0076f, 0.0002f, 0.0362f}, {0.0143f, 0.0004f, 0.0679f}, {0.0232f, 0.0006f, 0.1102f},
	{0.0435f, 0.0012f, 0.2074f}, {0.0776f, 0.0022f, 0.3713f}, {0.1344f, 0.0040f, 0.6456f},
	{0.2148f, 0.0073f, 1.0391f}, {0.2839f, 0.0116f, 1.3856f}, {0.3285f, 0.0168f, 1.6230f},
	{0.3483f, 0.0230f, 1.7471f}, {0.3481f, 0.0298f, 1.7826f}, {0.3362f, 0.0380f, 1.7721f},
	{0.3187f, 0.0480f, 1.7441f}, {0.2908f, 0.0600f, 1.6692f}, {0.2511f, 0.0739f, 1.5281f},
	{0.1954f, 0.0910f, 1.2876f}, {0.1421f, 0.1126f, 1.0419f}, {0.0956f, 0.1390f, 0.8130f},
	{0.0580f, 0.1693f, 0.6162f}, {0.0320f, 0.2080f, 0.4652f}, {0.0147f, 0.2586f, 0.3533f},
	{0.0049f, 0.3230f, 0.2720f}, {0.0024f, 0.4073f, 0.2123f}, {0.0093f, 0.5030f, 0.1582f},
	{0.0291f, 0.6082f, 0.1117f}, {0.0633f, 0.7100f, 0.0782f}, {0.1096f, 0.7932f, 0.0573f},
	{0.1655f, 0.8620f, 0.0422f}, {0.2257f, 0.9149f, 0.0298f}, {0.2904f, 0.9540f, 0.0203f},
	{0.3597f, 0.9803f, 0.0134f}, {0.4334f, 0.9950f, 0.0087f}, {0.5121f, 1.0000f, 0.0057f},
	{0.5945f, 0.9950f, 0.0039f}, {0.6784f, 0.9786f, 0.0027f}, {0.7621f, 0.9520f, 0.0021f},
	{0.8425f, 0.9154f, 0.0018f}, {0.9163f, 0.8700f, 0.0017f}, {0.9786f, 0.8163f, 0.0014f},
	{1.0263f, 0.7570f, 0.0011f}, {1.0567f, 0.6949f, 0.0010f}, {1.0622f, 0.6310f, 0.0008f},
	{1.0456f, 0.5668f, 0.0006f}, {1.0026f, 0.5030f, 0.0003f}, {0.9384f, 0.4412f, 0.0002f},
	{0.8544f, 0.3810f, 0.0002f}, {0.7514f, 0.3210f, 0.0001f}, {0.6424f, 0.2650f, 0.0000f},
	{0.5419f, 0.2170f, 0.0000f}, {0.4479f, 0.1750f, 0.0000f}, {0.3608f, 0.1382f, 0.0000f},
	{0.2835f, 0.1070f, 0.0000f}, {0.2187f, 0.0816f, 0.0000f}, {0.1649f, 0.0610f, 0.0000f},
	{0.1212f, 0.0446f, 0.0000f}, {0.0874f, 0.0320f, 0.0000f}, {0.0636f, 0.0232f, 0.0000f},
	{0.0468f, 0.0170f, 0.0000f}, {0.0329f, 0.0119f, 0.0000f}, {0.0227f, 0.0082f, 0.0000f},
	{0.0158f, 0.0057f, 0.0000f}, {0.0114f, 0.0041f, 0.0000f}, {0.0081f, 0.0029f, 0.0000f},
	{0.0058f, 0.0021f, 0.0000f}, {0.0041f, 0.0015f, 0.0000f}, {0.0029f, 0.0010f, 0.0000f},
	{0.0020f, 0.0007f, 0.0000f}, {0.0014f, 0.0005f, 0.0000f}, {0.0010f, 0.0004f, 0.0000f},
	{0.0007f, 0.0002f, 0.0000f}, {0.0005f, 0.0002f, 0.0000f}, {0.0003f, 0.0001f, 0.0000f},
	{0.0002f, 0.0001f, 0.0000f}, {0.0002f, 0.0001f, 0.0000f}, {0.0001f, 0.0000f, 0.0000f},
	{0.0001f, 0.0000f, 0.0000f}, {0.0001f, 0.0000f, 0.0000f}, {0.0000f, 0.0000f, 0.0000f} };

	void wavelength_to_xyz(float xyz[3], float lambda_nm)
	{
		float ii = (lambda_nm - 380.0f) * (1.0f / 5.0f); /* Scaled 0..80. */
		int i = (int)ii;

		if (i < 0 || i >= 80) {
			xyz[0] = 0.0f;
			xyz[1] = 0.0f;
			xyz[2] = 0.0f;
		}
		else {
			ii -= (float)i;
			const float* c = cie_colour_match[i];
			xyz[0] = c[0] + ii * (c[3] - c[0]);
			xyz[1] = c[1] + ii * (c[4] - c[1]);
			xyz[2] = c[2] + ii * (c[5] - c[2]);
		}
	}

	void wavelength_to_xyz_table(uint32_t** array, int* size) {
		
		*size = MAX_COLOR_BAND;
		*array = (uint32_t*)malloc(sizeof(uint32_t) * (*size));

		uint32_t* r_table = *array;
		int width = *size;

		for (int i = 0; i < width; i++) {
			float temperature = 380 + 400 / (float)width * (float)i;

			float rgb[3];
			wavelength_to_xyz(rgb, temperature);

			copy_v3_v3(&r_table[i], rgb);
		}
	}

	int SetWaveLength(const std::shared_ptr<cse::SharedState> shareState, int accessLayer) {

        uint32_t* array;
		int size;
		wavelength_to_xyz_table(&array, &size);

        accessLayer = GPU_color_band(shareState, size, array, accessLayer);

		//data->r[0] = xyz_to_rgb[0];
		//data->r[1] = xyz_to_rgb[3];
		//data->r[2] = xyz_to_rgb[6];
		//data->g[0] = xyz_to_rgb[1];
		//data->g[1] = xyz_to_rgb[4];
		//data->g[2] = xyz_to_rgb[7];
		//data->b[0] = xyz_to_rgb[2];
		//data->b[1] = xyz_to_rgb[5];
		//data->b[2] = xyz_to_rgb[8];

        return accessLayer;
	}
}
