int InterpretSPS (VideoParameters *p_Vid, DataPartition *p, seq_parameter_set_rbsp_t *sps)
{
  unsigned i;
  unsigned n_ScalingList;
  int reserved_zero;
  Bitstream *s = p->bitstream;
 
  assert (p != NULL);
  assert (p->bitstream != NULL);
  assert (p->bitstream->streamBuffer != 0);
  assert (sps != NULL);
 
  p_Dec->UsedBits = 0;
 
  sps->profile_idc                            = u_v  (8, "SPS: profile_idc"                           , s);
 
  if ((sps->profile_idc!=BASELINE       ) &&
      (sps->profile_idc!=MAIN           ) &&
      (sps->profile_idc!=EXTENDED       ) &&
      (sps->profile_idc!=FREXT_HP       ) &&
      (sps->profile_idc!=FREXT_Hi10P    ) &&
      (sps->profile_idc!=FREXT_Hi422    ) &&
      (sps->profile_idc!=FREXT_Hi444    ) &&
      (sps->profile_idc!=FREXT_CAVLC444 )
#if (MVC_EXTENSION_ENABLE)
      && (sps->profile_idc!=MVC_HIGH)
      && (sps->profile_idc!=STEREO_HIGH)
#endif
      )
  {
    printf("Invalid Profile IDC (%d) encountered. /n", sps->profile_idc);
    return p_Dec->UsedBits;
  }
 
  sps->constrained_set0_flag                  = u_1  (   "SPS: constrained_set0_flag"                 , s);
  sps->constrained_set1_flag                  = u_1  (   "SPS: constrained_set1_flag"                 , s);
  sps->constrained_set2_flag                  = u_1  (   "SPS: constrained_set2_flag"                 , s);
  sps->constrained_set3_flag                  = u_1  (   "SPS: constrained_set3_flag"                 , s);
#if (MVC_EXTENSION_ENABLE)
  sps->constrained_set4_flag                  = u_1  (   "SPS: constrained_set4_flag"                 , s);
  reserved_zero                               = u_v  (3, "SPS: reserved_zero_3bits"                   , s);
#else
  reserved_zero                               = u_v  (4, "SPS: reserved_zero_4bits"                   , s);
#endif
  assert (reserved_zero==0);
 
  sps->level_idc                              = u_v  (8, "SPS: level_idc"                             , s);
 
  sps->seq_parameter_set_id                   = ue_v ("SPS: seq_parameter_set_id"                     , s);
 
  // Fidelity Range Extensions stuff
  sps->chroma_format_idc = 1;
  sps->bit_depth_luma_minus8   = 0;
  sps->bit_depth_chroma_minus8 = 0;
  p_Vid->lossless_qpprime_flag   = 0;
  sps->separate_colour_plane_flag = 0;
 
  if((sps->profile_idc==FREXT_HP   ) ||
     (sps->profile_idc==FREXT_Hi10P) ||
     (sps->profile_idc==FREXT_Hi422) ||
     (sps->profile_idc==FREXT_Hi444) ||
     (sps->profile_idc==FREXT_CAVLC444)
#if (MVC_EXTENSION_ENABLE)
     || (sps->profile_idc==MVC_HIGH)
     || (sps->profile_idc==STEREO_HIGH)
#endif
     )
  {
    sps->chroma_format_idc                      = ue_v ("SPS: chroma_format_idc"                       , s);
 
    if(sps->chroma_format_idc == YUV444)
    {
      sps->separate_colour_plane_flag           = u_1  ("SPS: separate_colour_plane_flag"              , s);
    }
 
    sps->bit_depth_luma_minus8                  = ue_v ("SPS: bit_depth_luma_minus8"                   , s);
    sps->bit_depth_chroma_minus8                = ue_v ("SPS: bit_depth_chroma_minus8"                 , s);
    //checking;
    if((sps->bit_depth_luma_minus8+8 > sizeof(imgpel)*8) || (sps->bit_depth_chroma_minus8+8> sizeof(imgpel)*8))
      error ("Source picture has higher bit depth than imgpel data type. /nPlease recompile with larger data type for imgpel.", 500);
 
    p_Vid->lossless_qpprime_flag                  = u_1  ("SPS: lossless_qpprime_y_zero_flag"            , s);
 
    sps->seq_scaling_matrix_present_flag        = u_1  (   "SPS: seq_scaling_matrix_present_flag"       , s);
    
    if(sps->seq_scaling_matrix_present_flag)
    {
      n_ScalingList = (sps->chroma_format_idc != YUV444) ? 8 : 12;
      for(i=0; iseq_scaling_list_present_flag[i]   = u_1  (   "SPS: seq_scaling_list_present_flag"         , s);
        if(sps->seq_scaling_list_present_flag[i])
        {
          if(i<6) scaling_list="">ScalingList4x4[i], 16, &sps->UseDefaultScalingMatrix4x4Flag[i], s);
          else
            Scaling_List(sps->ScalingList8x8[i-6], 64, &sps->UseDefaultScalingMatrix8x8Flag[i-6], s);
        }
      }
    }
  }
 
  sps->log2_max_frame_num_minus4              = ue_v ("SPS: log2_max_frame_num_minus4"                , s);
  sps->pic_order_cnt_type                     = ue_v ("SPS: pic_order_cnt_type"                       , s);
 
  if (sps->pic_order_cnt_type == 0)
    sps->log2_max_pic_order_cnt_lsb_minus4 = ue_v ("SPS: log2_max_pic_order_cnt_lsb_minus4"           , s);
  else if (sps->pic_order_cnt_type == 1)
  {
    sps->delta_pic_order_always_zero_flag      = u_1  ("SPS: delta_pic_order_always_zero_flag"       , s);
    sps->offset_for_non_ref_pic                = se_v ("SPS: offset_for_non_ref_pic"                 , s);
    sps->offset_for_top_to_bottom_field        = se_v ("SPS: offset_for_top_to_bottom_field"         , s);
    sps->num_ref_frames_in_pic_order_cnt_cycle = ue_v ("SPS: num_ref_frames_in_pic_order_cnt_cycle"  , s);
    for(i=0; inum_ref_frames_in_pic_order_cnt_cycle; i++)
      sps->offset_for_ref_frame[i]               = se_v ("SPS: offset_for_ref_frame[i]"              , s);
  }
  sps->num_ref_frames                        = ue_v ("SPS: num_ref_frames"                         , s);
  sps->gaps_in_frame_num_value_allowed_flag  = u_1  ("SPS: gaps_in_frame_num_value_allowed_flag"   , s);
  sps->pic_width_in_mbs_minus1               = ue_v ("SPS: pic_width_in_mbs_minus1"                , s);
  sps->pic_height_in_map_units_minus1        = ue_v ("SPS: pic_height_in_map_units_minus1"         , s);
  sps->frame_mbs_only_flag                   = u_1  ("SPS: frame_mbs_only_flag"                    , s);
  if (!sps->frame_mbs_only_flag)
  {
    sps->mb_adaptive_frame_field_flag        = u_1  ("SPS: mb_adaptive_frame_field_flag"           , s);
  }
  sps->direct_8x8_inference_flag             = u_1  ("SPS: direct_8x8_inference_flag"              , s);
  sps->frame_cropping_flag                   = u_1  ("SPS: frame_cropping_flag"                    , s);
 
  if (sps->frame_cropping_flag)
  {
    sps->frame_cropping_rect_left_offset      = ue_v ("SPS: frame_cropping_rect_left_offset"           , s);
    sps->frame_cropping_rect_right_offset     = ue_v ("SPS: frame_cropping_rect_right_offset"          , s);
    sps->frame_cropping_rect_top_offset       = ue_v ("SPS: frame_cropping_rect_top_offset"            , s);
    sps->frame_cropping_rect_bottom_offset    = ue_v ("SPS: frame_cropping_rect_bottom_offset"         , s);
  }
  sps->vui_parameters_present_flag           = (Boolean) u_1  ("SPS: vui_parameters_present_flag"      , s);
 
  InitVUI(sps);
  ReadVUI(p, sps);
 
  sps->Valid = TRUE;
  return p_Dec->UsedBits;
}
