int InterpretPPS (VideoParameters *p_Vid, DataPartition *p, pic_parameter_set_rbsp_t *pps)
{
  unsigned i;
  unsigned n_ScalingList;
  int chroma_format_idc;
  int NumberBitsPerSliceGroupId;
  Bitstream *s = p->bitstream;
 
  assert (p != NULL);
  assert (p->bitstream != NULL);
  assert (p->bitstream->streamBuffer != 0);
  assert (pps != NULL);
 
  p_Dec->UsedBits = 0;
 
  pps->pic_parameter_set_id                  = ue_v ("PPS: pic_parameter_set_id"                   , s);
  pps->seq_parameter_set_id                  = ue_v ("PPS: seq_parameter_set_id"                   , s);
  pps->entropy_coding_mode_flag              = u_1  ("PPS: entropy_coding_mode_flag"               , s);
 
  //! Note: as per JVT-F078 the following bit is unconditional.  If F078 is not accepted, then
  //! one has to fetch the correct SPS to check whether the bit is present (hopefully there is
  //! no consistency problem :-(
  //! The current encoder code handles this in the same way.  When you change this, don't forget
  //! the encoder!  StW, 12/8/02
  pps->bottom_field_pic_order_in_frame_present_flag                = u_1  ("PPS: bottom_field_pic_order_in_frame_present_flag"                 , s);
 
  pps->num_slice_groups_minus1               = ue_v ("PPS: num_slice_groups_minus1"                , s);
 
  // FMO stuff begins here
  if (pps->num_slice_groups_minus1 > 0)
  {
    pps->slice_group_map_type               = ue_v ("PPS: slice_group_map_type"                , s);
    if (pps->slice_group_map_type == 0)
    {
      for (i=0; i<=pps->num_slice_groups_minus1; i++)
        pps->run_length_minus1 [i]                  = ue_v ("PPS: run_length_minus1 [i]"              , s);
    }
    else if (pps->slice_group_map_type == 2)
    {
      for (i=0; inum_slice_groups_minus1; i++)
      {
        //! JVT-F078: avoid reference of SPS by using ue(v) instead of u(v)
        pps->top_left [i]                          = ue_v ("PPS: top_left [i]"                        , s);
        pps->bottom_right [i]                      = ue_v ("PPS: bottom_right [i]"                    , s);
      }
    }
    else if (pps->slice_group_map_type == 3 ||
             pps->slice_group_map_type == 4 ||
             pps->slice_group_map_type == 5)
    {
      pps->slice_group_change_direction_flag     = u_1  ("PPS: slice_group_change_direction_flag"      , s);
      pps->slice_group_change_rate_minus1        = ue_v ("PPS: slice_group_change_rate_minus1"         , s);
    }
    else if (pps->slice_group_map_type == 6)
    {
      if (pps->num_slice_groups_minus1+1 >4)
        NumberBitsPerSliceGroupId = 3;
      else if (pps->num_slice_groups_minus1+1 > 2)
        NumberBitsPerSliceGroupId = 2;
      else
        NumberBitsPerSliceGroupId = 1;
      pps->pic_size_in_map_units_minus1      = ue_v ("PPS: pic_size_in_map_units_minus1"               , s);
      if ((pps->slice_group_id = calloc (pps->pic_size_in_map_units_minus1+1, 1)) == NULL)
        no_mem_exit ("InterpretPPS: slice_group_id");
      for (i=0; i<=pps->pic_size_in_map_units_minus1; i++)
        pps->slice_group_id[i] = (byte) u_v (NumberBitsPerSliceGroupId, "slice_group_id[i]", s);
    }
  }
 
  // End of FMO stuff
 
  pps->num_ref_idx_l0_active_minus1          = ue_v ("PPS: num_ref_idx_l0_active_minus1"           , s);
  pps->num_ref_idx_l1_active_minus1          = ue_v ("PPS: num_ref_idx_l1_active_minus1"           , s);
  pps->weighted_pred_flag                    = u_1  ("PPS: weighted_pred_flag"                     , s);
  pps->weighted_bipred_idc                   = u_v  ( 2, "PPS: weighted_bipred_idc"                , s);
  pps->pic_init_qp_minus26                   = se_v ("PPS: pic_init_qp_minus26"                    , s);
  pps->pic_init_qs_minus26                   = se_v ("PPS: pic_init_qs_minus26"                    , s);
 
  pps->chroma_qp_index_offset                = se_v ("PPS: chroma_qp_index_offset"                 , s);
 
  pps->deblocking_filter_control_present_flag = u_1 ("PPS: deblocking_filter_control_present_flag" , s);
  pps->constrained_intra_pred_flag           = u_1  ("PPS: constrained_intra_pred_flag"            , s);
  pps->redundant_pic_cnt_present_flag        = u_1  ("PPS: redundant_pic_cnt_present_flag"         , s);
 
  if(more_rbsp_data(s->streamBuffer, s->frame_bitoffset,s->bitstream_length)) // more_data_in_rbsp()
  {
    //Fidelity Range Extensions Stuff
    pps->transform_8x8_mode_flag           = u_1  ("PPS: transform_8x8_mode_flag"                , s);
    pps->pic_scaling_matrix_present_flag   =  u_1  ("PPS: pic_scaling_matrix_present_flag"        , s);
 
    if(pps->pic_scaling_matrix_present_flag)
    {
      chroma_format_idc = p_Vid->SeqParSet[pps->seq_parameter_set_id].chroma_format_idc;
      n_ScalingList = 6 + ((chroma_format_idc != YUV444) ? 2 : 6) * pps->transform_8x8_mode_flag;
      for(i=0; ipic_scaling_list_present_flag[i]= u_1  ("PPS: pic_scaling_list_present_flag"          , s);
 
        if(pps->pic_scaling_list_present_flag[i])
        {
          if(i<6) scaling_list="">ScalingList4x4[i], 16, &pps->UseDefaultScalingMatrix4x4Flag[i], s);
          else
            Scaling_List(pps->ScalingList8x8[i-6], 64, &pps->UseDefaultScalingMatrix8x8Flag[i-6], s);
        }
      }
    }
    pps->second_chroma_qp_index_offset      = se_v ("PPS: second_chroma_qp_index_offset"          , s);
  }
  else
  {
    pps->second_chroma_qp_index_offset      = pps->chroma_qp_index_offset;
  }
 
  pps->Valid = TRUE;
  return p_Dec->UsedBits;
}
