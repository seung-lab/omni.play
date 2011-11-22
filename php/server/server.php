<?php
/**
 * Autogenerated by Thrift Compiler (0.7.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 */
include_once $GLOBALS['THRIFT_ROOT'].'/Thrift.php';

include_once $GLOBALS['THRIFT_ROOT'].'/packages/server/server_types.php';

interface serverIf {
  public function get_chan_tile($vol, $point, $mipLevel, $view);
  public function get_seg_tiles($vol, $segId, $segBbox, $mipLevel, $view);
  public function get_seg_id($vol, $point);
  public function get_seg_ids($vol, $point, $radius, $view);
  public function compare_results($old_results, $new_result);
}

class serverClient implements serverIf {
  protected $input_ = null;
  protected $output_ = null;

  protected $seqid_ = 0;

  public function __construct($input, $output=null) {
    $this->input_ = $input;
    $this->output_ = $output ? $output : $input;
  }

  public function get_chan_tile($vol, $point, $mipLevel, $view)
  {
    $this->send_get_chan_tile($vol, $point, $mipLevel, $view);
    return $this->recv_get_chan_tile();
  }

  public function send_get_chan_tile($vol, $point, $mipLevel, $view)
  {
    $args = new server_get_chan_tile_args();
    $args->vol = $vol;
    $args->point = $point;
    $args->mipLevel = $mipLevel;
    $args->view = $view;
    $bin_accel = ($this->output_ instanceof TProtocol::$TBINARYPROTOCOLACCELERATED) && function_exists('thrift_protocol_write_binary');
    if ($bin_accel)
    {
      thrift_protocol_write_binary($this->output_, 'get_chan_tile', TMessageType::CALL, $args, $this->seqid_, $this->output_->isStrictWrite());
    }
    else
    {
      $this->output_->writeMessageBegin('get_chan_tile', TMessageType::CALL, $this->seqid_);
      $args->write($this->output_);
      $this->output_->writeMessageEnd();
      $this->output_->getTransport()->flush();
    }
  }

  public function recv_get_chan_tile()
  {
    $bin_accel = ($this->input_ instanceof TProtocol::$TBINARYPROTOCOLACCELERATED) && function_exists('thrift_protocol_read_binary');
    if ($bin_accel) $result = thrift_protocol_read_binary($this->input_, 'server_get_chan_tile_result', $this->input_->isStrictRead());
    else
    {
      $rseqid = 0;
      $fname = null;
      $mtype = 0;

      $this->input_->readMessageBegin($fname, $mtype, $rseqid);
      if ($mtype == TMessageType::EXCEPTION) {
        $x = new TApplicationException();
        $x->read($this->input_);
        $this->input_->readMessageEnd();
        throw $x;
      }
      $result = new server_get_chan_tile_result();
      $result->read($this->input_);
      $this->input_->readMessageEnd();
    }
    if ($result->success !== null) {
      return $result->success;
    }
    throw new Exception("get_chan_tile failed: unknown result");
  }

  public function get_seg_tiles($vol, $segId, $segBbox, $mipLevel, $view)
  {
    $this->send_get_seg_tiles($vol, $segId, $segBbox, $mipLevel, $view);
    return $this->recv_get_seg_tiles();
  }

  public function send_get_seg_tiles($vol, $segId, $segBbox, $mipLevel, $view)
  {
    $args = new server_get_seg_tiles_args();
    $args->vol = $vol;
    $args->segId = $segId;
    $args->segBbox = $segBbox;
    $args->mipLevel = $mipLevel;
    $args->view = $view;
    $bin_accel = ($this->output_ instanceof TProtocol::$TBINARYPROTOCOLACCELERATED) && function_exists('thrift_protocol_write_binary');
    if ($bin_accel)
    {
      thrift_protocol_write_binary($this->output_, 'get_seg_tiles', TMessageType::CALL, $args, $this->seqid_, $this->output_->isStrictWrite());
    }
    else
    {
      $this->output_->writeMessageBegin('get_seg_tiles', TMessageType::CALL, $this->seqid_);
      $args->write($this->output_);
      $this->output_->writeMessageEnd();
      $this->output_->getTransport()->flush();
    }
  }

  public function recv_get_seg_tiles()
  {
    $bin_accel = ($this->input_ instanceof TProtocol::$TBINARYPROTOCOLACCELERATED) && function_exists('thrift_protocol_read_binary');
    if ($bin_accel) $result = thrift_protocol_read_binary($this->input_, 'server_get_seg_tiles_result', $this->input_->isStrictRead());
    else
    {
      $rseqid = 0;
      $fname = null;
      $mtype = 0;

      $this->input_->readMessageBegin($fname, $mtype, $rseqid);
      if ($mtype == TMessageType::EXCEPTION) {
        $x = new TApplicationException();
        $x->read($this->input_);
        $this->input_->readMessageEnd();
        throw $x;
      }
      $result = new server_get_seg_tiles_result();
      $result->read($this->input_);
      $this->input_->readMessageEnd();
    }
    if ($result->success !== null) {
      return $result->success;
    }
    throw new Exception("get_seg_tiles failed: unknown result");
  }

  public function get_seg_id($vol, $point)
  {
    $this->send_get_seg_id($vol, $point);
    return $this->recv_get_seg_id();
  }

  public function send_get_seg_id($vol, $point)
  {
    $args = new server_get_seg_id_args();
    $args->vol = $vol;
    $args->point = $point;
    $bin_accel = ($this->output_ instanceof TProtocol::$TBINARYPROTOCOLACCELERATED) && function_exists('thrift_protocol_write_binary');
    if ($bin_accel)
    {
      thrift_protocol_write_binary($this->output_, 'get_seg_id', TMessageType::CALL, $args, $this->seqid_, $this->output_->isStrictWrite());
    }
    else
    {
      $this->output_->writeMessageBegin('get_seg_id', TMessageType::CALL, $this->seqid_);
      $args->write($this->output_);
      $this->output_->writeMessageEnd();
      $this->output_->getTransport()->flush();
    }
  }

  public function recv_get_seg_id()
  {
    $bin_accel = ($this->input_ instanceof TProtocol::$TBINARYPROTOCOLACCELERATED) && function_exists('thrift_protocol_read_binary');
    if ($bin_accel) $result = thrift_protocol_read_binary($this->input_, 'server_get_seg_id_result', $this->input_->isStrictRead());
    else
    {
      $rseqid = 0;
      $fname = null;
      $mtype = 0;

      $this->input_->readMessageBegin($fname, $mtype, $rseqid);
      if ($mtype == TMessageType::EXCEPTION) {
        $x = new TApplicationException();
        $x->read($this->input_);
        $this->input_->readMessageEnd();
        throw $x;
      }
      $result = new server_get_seg_id_result();
      $result->read($this->input_);
      $this->input_->readMessageEnd();
    }
    if ($result->success !== null) {
      return $result->success;
    }
    throw new Exception("get_seg_id failed: unknown result");
  }

  public function get_seg_ids($vol, $point, $radius, $view)
  {
    $this->send_get_seg_ids($vol, $point, $radius, $view);
    return $this->recv_get_seg_ids();
  }

  public function send_get_seg_ids($vol, $point, $radius, $view)
  {
    $args = new server_get_seg_ids_args();
    $args->vol = $vol;
    $args->point = $point;
    $args->radius = $radius;
    $args->view = $view;
    $bin_accel = ($this->output_ instanceof TProtocol::$TBINARYPROTOCOLACCELERATED) && function_exists('thrift_protocol_write_binary');
    if ($bin_accel)
    {
      thrift_protocol_write_binary($this->output_, 'get_seg_ids', TMessageType::CALL, $args, $this->seqid_, $this->output_->isStrictWrite());
    }
    else
    {
      $this->output_->writeMessageBegin('get_seg_ids', TMessageType::CALL, $this->seqid_);
      $args->write($this->output_);
      $this->output_->writeMessageEnd();
      $this->output_->getTransport()->flush();
    }
  }

  public function recv_get_seg_ids()
  {
    $bin_accel = ($this->input_ instanceof TProtocol::$TBINARYPROTOCOLACCELERATED) && function_exists('thrift_protocol_read_binary');
    if ($bin_accel) $result = thrift_protocol_read_binary($this->input_, 'server_get_seg_ids_result', $this->input_->isStrictRead());
    else
    {
      $rseqid = 0;
      $fname = null;
      $mtype = 0;

      $this->input_->readMessageBegin($fname, $mtype, $rseqid);
      if ($mtype == TMessageType::EXCEPTION) {
        $x = new TApplicationException();
        $x->read($this->input_);
        $this->input_->readMessageEnd();
        throw $x;
      }
      $result = new server_get_seg_ids_result();
      $result->read($this->input_);
      $this->input_->readMessageEnd();
    }
    if ($result->success !== null) {
      return $result->success;
    }
    throw new Exception("get_seg_ids failed: unknown result");
  }

  public function compare_results($old_results, $new_result)
  {
    $this->send_compare_results($old_results, $new_result);
    return $this->recv_compare_results();
  }

  public function send_compare_results($old_results, $new_result)
  {
    $args = new server_compare_results_args();
    $args->old_results = $old_results;
    $args->new_result = $new_result;
    $bin_accel = ($this->output_ instanceof TProtocol::$TBINARYPROTOCOLACCELERATED) && function_exists('thrift_protocol_write_binary');
    if ($bin_accel)
    {
      thrift_protocol_write_binary($this->output_, 'compare_results', TMessageType::CALL, $args, $this->seqid_, $this->output_->isStrictWrite());
    }
    else
    {
      $this->output_->writeMessageBegin('compare_results', TMessageType::CALL, $this->seqid_);
      $args->write($this->output_);
      $this->output_->writeMessageEnd();
      $this->output_->getTransport()->flush();
    }
  }

  public function recv_compare_results()
  {
    $bin_accel = ($this->input_ instanceof TProtocol::$TBINARYPROTOCOLACCELERATED) && function_exists('thrift_protocol_read_binary');
    if ($bin_accel) $result = thrift_protocol_read_binary($this->input_, 'server_compare_results_result', $this->input_->isStrictRead());
    else
    {
      $rseqid = 0;
      $fname = null;
      $mtype = 0;

      $this->input_->readMessageBegin($fname, $mtype, $rseqid);
      if ($mtype == TMessageType::EXCEPTION) {
        $x = new TApplicationException();
        $x->read($this->input_);
        $this->input_->readMessageEnd();
        throw $x;
      }
      $result = new server_compare_results_result();
      $result->read($this->input_);
      $this->input_->readMessageEnd();
    }
    if ($result->success !== null) {
      return $result->success;
    }
    throw new Exception("compare_results failed: unknown result");
  }

}

// HELPER FUNCTIONS AND STRUCTURES

class server_get_chan_tile_args {
  static $_TSPEC;

  public $vol = null;
  public $point = null;
  public $mipLevel = null;
  public $view = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'vol',
          'type' => TType::STRUCT,
          'class' => 'metadata',
          ),
        2 => array(
          'var' => 'point',
          'type' => TType::STRUCT,
          'class' => 'vector3d',
          ),
        3 => array(
          'var' => 'mipLevel',
          'type' => TType::I32,
          ),
        4 => array(
          'var' => 'view',
          'type' => TType::I32,
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['vol'])) {
        $this->vol = $vals['vol'];
      }
      if (isset($vals['point'])) {
        $this->point = $vals['point'];
      }
      if (isset($vals['mipLevel'])) {
        $this->mipLevel = $vals['mipLevel'];
      }
      if (isset($vals['view'])) {
        $this->view = $vals['view'];
      }
    }
  }

  public function getName() {
    return 'server_get_chan_tile_args';
  }

  public function read($input)
  {
    $xfer = 0;
    $fname = null;
    $ftype = 0;
    $fid = 0;
    $xfer += $input->readStructBegin($fname);
    while (true)
    {
      $xfer += $input->readFieldBegin($fname, $ftype, $fid);
      if ($ftype == TType::STOP) {
        break;
      }
      switch ($fid)
      {
        case 1:
          if ($ftype == TType::STRUCT) {
            $this->vol = new metadata();
            $xfer += $this->vol->read($input);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 2:
          if ($ftype == TType::STRUCT) {
            $this->point = new vector3d();
            $xfer += $this->point->read($input);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 3:
          if ($ftype == TType::I32) {
            $xfer += $input->readI32($this->mipLevel);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 4:
          if ($ftype == TType::I32) {
            $xfer += $input->readI32($this->view);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        default:
          $xfer += $input->skip($ftype);
          break;
      }
      $xfer += $input->readFieldEnd();
    }
    $xfer += $input->readStructEnd();
    return $xfer;
  }

  public function write($output) {
    $xfer = 0;
    $xfer += $output->writeStructBegin('server_get_chan_tile_args');
    if ($this->vol !== null) {
      if (!is_object($this->vol)) {
        throw new TProtocolException('Bad type in structure.', TProtocolException::INVALID_DATA);
      }
      $xfer += $output->writeFieldBegin('vol', TType::STRUCT, 1);
      $xfer += $this->vol->write($output);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->point !== null) {
      if (!is_object($this->point)) {
        throw new TProtocolException('Bad type in structure.', TProtocolException::INVALID_DATA);
      }
      $xfer += $output->writeFieldBegin('point', TType::STRUCT, 2);
      $xfer += $this->point->write($output);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->mipLevel !== null) {
      $xfer += $output->writeFieldBegin('mipLevel', TType::I32, 3);
      $xfer += $output->writeI32($this->mipLevel);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->view !== null) {
      $xfer += $output->writeFieldBegin('view', TType::I32, 4);
      $xfer += $output->writeI32($this->view);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class server_get_chan_tile_result {
  static $_TSPEC;

  public $success = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        0 => array(
          'var' => 'success',
          'type' => TType::STRUCT,
          'class' => 'tile',
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['success'])) {
        $this->success = $vals['success'];
      }
    }
  }

  public function getName() {
    return 'server_get_chan_tile_result';
  }

  public function read($input)
  {
    $xfer = 0;
    $fname = null;
    $ftype = 0;
    $fid = 0;
    $xfer += $input->readStructBegin($fname);
    while (true)
    {
      $xfer += $input->readFieldBegin($fname, $ftype, $fid);
      if ($ftype == TType::STOP) {
        break;
      }
      switch ($fid)
      {
        case 0:
          if ($ftype == TType::STRUCT) {
            $this->success = new tile();
            $xfer += $this->success->read($input);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        default:
          $xfer += $input->skip($ftype);
          break;
      }
      $xfer += $input->readFieldEnd();
    }
    $xfer += $input->readStructEnd();
    return $xfer;
  }

  public function write($output) {
    $xfer = 0;
    $xfer += $output->writeStructBegin('server_get_chan_tile_result');
    if ($this->success !== null) {
      if (!is_object($this->success)) {
        throw new TProtocolException('Bad type in structure.', TProtocolException::INVALID_DATA);
      }
      $xfer += $output->writeFieldBegin('success', TType::STRUCT, 0);
      $xfer += $this->success->write($output);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class server_get_seg_tiles_args {
  static $_TSPEC;

  public $vol = null;
  public $segId = null;
  public $segBbox = null;
  public $mipLevel = null;
  public $view = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'vol',
          'type' => TType::STRUCT,
          'class' => 'metadata',
          ),
        2 => array(
          'var' => 'segId',
          'type' => TType::I32,
          ),
        3 => array(
          'var' => 'segBbox',
          'type' => TType::STRUCT,
          'class' => 'bbox',
          ),
        4 => array(
          'var' => 'mipLevel',
          'type' => TType::I32,
          ),
        5 => array(
          'var' => 'view',
          'type' => TType::I32,
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['vol'])) {
        $this->vol = $vals['vol'];
      }
      if (isset($vals['segId'])) {
        $this->segId = $vals['segId'];
      }
      if (isset($vals['segBbox'])) {
        $this->segBbox = $vals['segBbox'];
      }
      if (isset($vals['mipLevel'])) {
        $this->mipLevel = $vals['mipLevel'];
      }
      if (isset($vals['view'])) {
        $this->view = $vals['view'];
      }
    }
  }

  public function getName() {
    return 'server_get_seg_tiles_args';
  }

  public function read($input)
  {
    $xfer = 0;
    $fname = null;
    $ftype = 0;
    $fid = 0;
    $xfer += $input->readStructBegin($fname);
    while (true)
    {
      $xfer += $input->readFieldBegin($fname, $ftype, $fid);
      if ($ftype == TType::STOP) {
        break;
      }
      switch ($fid)
      {
        case 1:
          if ($ftype == TType::STRUCT) {
            $this->vol = new metadata();
            $xfer += $this->vol->read($input);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 2:
          if ($ftype == TType::I32) {
            $xfer += $input->readI32($this->segId);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 3:
          if ($ftype == TType::STRUCT) {
            $this->segBbox = new bbox();
            $xfer += $this->segBbox->read($input);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 4:
          if ($ftype == TType::I32) {
            $xfer += $input->readI32($this->mipLevel);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 5:
          if ($ftype == TType::I32) {
            $xfer += $input->readI32($this->view);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        default:
          $xfer += $input->skip($ftype);
          break;
      }
      $xfer += $input->readFieldEnd();
    }
    $xfer += $input->readStructEnd();
    return $xfer;
  }

  public function write($output) {
    $xfer = 0;
    $xfer += $output->writeStructBegin('server_get_seg_tiles_args');
    if ($this->vol !== null) {
      if (!is_object($this->vol)) {
        throw new TProtocolException('Bad type in structure.', TProtocolException::INVALID_DATA);
      }
      $xfer += $output->writeFieldBegin('vol', TType::STRUCT, 1);
      $xfer += $this->vol->write($output);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->segId !== null) {
      $xfer += $output->writeFieldBegin('segId', TType::I32, 2);
      $xfer += $output->writeI32($this->segId);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->segBbox !== null) {
      if (!is_object($this->segBbox)) {
        throw new TProtocolException('Bad type in structure.', TProtocolException::INVALID_DATA);
      }
      $xfer += $output->writeFieldBegin('segBbox', TType::STRUCT, 3);
      $xfer += $this->segBbox->write($output);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->mipLevel !== null) {
      $xfer += $output->writeFieldBegin('mipLevel', TType::I32, 4);
      $xfer += $output->writeI32($this->mipLevel);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->view !== null) {
      $xfer += $output->writeFieldBegin('view', TType::I32, 5);
      $xfer += $output->writeI32($this->view);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class server_get_seg_tiles_result {
  static $_TSPEC;

  public $success = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        0 => array(
          'var' => 'success',
          'type' => TType::LST,
          'etype' => TType::STRUCT,
          'elem' => array(
            'type' => TType::STRUCT,
            'class' => 'tile',
            ),
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['success'])) {
        $this->success = $vals['success'];
      }
    }
  }

  public function getName() {
    return 'server_get_seg_tiles_result';
  }

  public function read($input)
  {
    $xfer = 0;
    $fname = null;
    $ftype = 0;
    $fid = 0;
    $xfer += $input->readStructBegin($fname);
    while (true)
    {
      $xfer += $input->readFieldBegin($fname, $ftype, $fid);
      if ($ftype == TType::STOP) {
        break;
      }
      switch ($fid)
      {
        case 0:
          if ($ftype == TType::LST) {
            $this->success = array();
            $_size7 = 0;
            $_etype10 = 0;
            $xfer += $input->readListBegin($_etype10, $_size7);
            for ($_i11 = 0; $_i11 < $_size7; ++$_i11)
            {
              $elem12 = null;
              $elem12 = new tile();
              $xfer += $elem12->read($input);
              $this->success []= $elem12;
            }
            $xfer += $input->readListEnd();
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        default:
          $xfer += $input->skip($ftype);
          break;
      }
      $xfer += $input->readFieldEnd();
    }
    $xfer += $input->readStructEnd();
    return $xfer;
  }

  public function write($output) {
    $xfer = 0;
    $xfer += $output->writeStructBegin('server_get_seg_tiles_result');
    if ($this->success !== null) {
      if (!is_array($this->success)) {
        throw new TProtocolException('Bad type in structure.', TProtocolException::INVALID_DATA);
      }
      $xfer += $output->writeFieldBegin('success', TType::LST, 0);
      {
        $output->writeListBegin(TType::STRUCT, count($this->success));
        {
          foreach ($this->success as $iter13)
          {
            $xfer += $iter13->write($output);
          }
        }
        $output->writeListEnd();
      }
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class server_get_seg_id_args {
  static $_TSPEC;

  public $vol = null;
  public $point = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'vol',
          'type' => TType::STRUCT,
          'class' => 'metadata',
          ),
        2 => array(
          'var' => 'point',
          'type' => TType::STRUCT,
          'class' => 'vector3d',
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['vol'])) {
        $this->vol = $vals['vol'];
      }
      if (isset($vals['point'])) {
        $this->point = $vals['point'];
      }
    }
  }

  public function getName() {
    return 'server_get_seg_id_args';
  }

  public function read($input)
  {
    $xfer = 0;
    $fname = null;
    $ftype = 0;
    $fid = 0;
    $xfer += $input->readStructBegin($fname);
    while (true)
    {
      $xfer += $input->readFieldBegin($fname, $ftype, $fid);
      if ($ftype == TType::STOP) {
        break;
      }
      switch ($fid)
      {
        case 1:
          if ($ftype == TType::STRUCT) {
            $this->vol = new metadata();
            $xfer += $this->vol->read($input);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 2:
          if ($ftype == TType::STRUCT) {
            $this->point = new vector3d();
            $xfer += $this->point->read($input);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        default:
          $xfer += $input->skip($ftype);
          break;
      }
      $xfer += $input->readFieldEnd();
    }
    $xfer += $input->readStructEnd();
    return $xfer;
  }

  public function write($output) {
    $xfer = 0;
    $xfer += $output->writeStructBegin('server_get_seg_id_args');
    if ($this->vol !== null) {
      if (!is_object($this->vol)) {
        throw new TProtocolException('Bad type in structure.', TProtocolException::INVALID_DATA);
      }
      $xfer += $output->writeFieldBegin('vol', TType::STRUCT, 1);
      $xfer += $this->vol->write($output);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->point !== null) {
      if (!is_object($this->point)) {
        throw new TProtocolException('Bad type in structure.', TProtocolException::INVALID_DATA);
      }
      $xfer += $output->writeFieldBegin('point', TType::STRUCT, 2);
      $xfer += $this->point->write($output);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class server_get_seg_id_result {
  static $_TSPEC;

  public $success = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        0 => array(
          'var' => 'success',
          'type' => TType::I32,
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['success'])) {
        $this->success = $vals['success'];
      }
    }
  }

  public function getName() {
    return 'server_get_seg_id_result';
  }

  public function read($input)
  {
    $xfer = 0;
    $fname = null;
    $ftype = 0;
    $fid = 0;
    $xfer += $input->readStructBegin($fname);
    while (true)
    {
      $xfer += $input->readFieldBegin($fname, $ftype, $fid);
      if ($ftype == TType::STOP) {
        break;
      }
      switch ($fid)
      {
        case 0:
          if ($ftype == TType::I32) {
            $xfer += $input->readI32($this->success);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        default:
          $xfer += $input->skip($ftype);
          break;
      }
      $xfer += $input->readFieldEnd();
    }
    $xfer += $input->readStructEnd();
    return $xfer;
  }

  public function write($output) {
    $xfer = 0;
    $xfer += $output->writeStructBegin('server_get_seg_id_result');
    if ($this->success !== null) {
      $xfer += $output->writeFieldBegin('success', TType::I32, 0);
      $xfer += $output->writeI32($this->success);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class server_get_seg_ids_args {
  static $_TSPEC;

  public $vol = null;
  public $point = null;
  public $radius = null;
  public $view = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'vol',
          'type' => TType::STRUCT,
          'class' => 'metadata',
          ),
        2 => array(
          'var' => 'point',
          'type' => TType::STRUCT,
          'class' => 'vector3d',
          ),
        3 => array(
          'var' => 'radius',
          'type' => TType::DOUBLE,
          ),
        4 => array(
          'var' => 'view',
          'type' => TType::I32,
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['vol'])) {
        $this->vol = $vals['vol'];
      }
      if (isset($vals['point'])) {
        $this->point = $vals['point'];
      }
      if (isset($vals['radius'])) {
        $this->radius = $vals['radius'];
      }
      if (isset($vals['view'])) {
        $this->view = $vals['view'];
      }
    }
  }

  public function getName() {
    return 'server_get_seg_ids_args';
  }

  public function read($input)
  {
    $xfer = 0;
    $fname = null;
    $ftype = 0;
    $fid = 0;
    $xfer += $input->readStructBegin($fname);
    while (true)
    {
      $xfer += $input->readFieldBegin($fname, $ftype, $fid);
      if ($ftype == TType::STOP) {
        break;
      }
      switch ($fid)
      {
        case 1:
          if ($ftype == TType::STRUCT) {
            $this->vol = new metadata();
            $xfer += $this->vol->read($input);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 2:
          if ($ftype == TType::STRUCT) {
            $this->point = new vector3d();
            $xfer += $this->point->read($input);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 3:
          if ($ftype == TType::DOUBLE) {
            $xfer += $input->readDouble($this->radius);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 4:
          if ($ftype == TType::I32) {
            $xfer += $input->readI32($this->view);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        default:
          $xfer += $input->skip($ftype);
          break;
      }
      $xfer += $input->readFieldEnd();
    }
    $xfer += $input->readStructEnd();
    return $xfer;
  }

  public function write($output) {
    $xfer = 0;
    $xfer += $output->writeStructBegin('server_get_seg_ids_args');
    if ($this->vol !== null) {
      if (!is_object($this->vol)) {
        throw new TProtocolException('Bad type in structure.', TProtocolException::INVALID_DATA);
      }
      $xfer += $output->writeFieldBegin('vol', TType::STRUCT, 1);
      $xfer += $this->vol->write($output);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->point !== null) {
      if (!is_object($this->point)) {
        throw new TProtocolException('Bad type in structure.', TProtocolException::INVALID_DATA);
      }
      $xfer += $output->writeFieldBegin('point', TType::STRUCT, 2);
      $xfer += $this->point->write($output);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->radius !== null) {
      $xfer += $output->writeFieldBegin('radius', TType::DOUBLE, 3);
      $xfer += $output->writeDouble($this->radius);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->view !== null) {
      $xfer += $output->writeFieldBegin('view', TType::I32, 4);
      $xfer += $output->writeI32($this->view);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class server_get_seg_ids_result {
  static $_TSPEC;

  public $success = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        0 => array(
          'var' => 'success',
          'type' => TType::LST,
          'etype' => TType::I32,
          'elem' => array(
            'type' => TType::I32,
            ),
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['success'])) {
        $this->success = $vals['success'];
      }
    }
  }

  public function getName() {
    return 'server_get_seg_ids_result';
  }

  public function read($input)
  {
    $xfer = 0;
    $fname = null;
    $ftype = 0;
    $fid = 0;
    $xfer += $input->readStructBegin($fname);
    while (true)
    {
      $xfer += $input->readFieldBegin($fname, $ftype, $fid);
      if ($ftype == TType::STOP) {
        break;
      }
      switch ($fid)
      {
        case 0:
          if ($ftype == TType::LST) {
            $this->success = array();
            $_size14 = 0;
            $_etype17 = 0;
            $xfer += $input->readListBegin($_etype17, $_size14);
            for ($_i18 = 0; $_i18 < $_size14; ++$_i18)
            {
              $elem19 = null;
              $xfer += $input->readI32($elem19);
              $this->success []= $elem19;
            }
            $xfer += $input->readListEnd();
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        default:
          $xfer += $input->skip($ftype);
          break;
      }
      $xfer += $input->readFieldEnd();
    }
    $xfer += $input->readStructEnd();
    return $xfer;
  }

  public function write($output) {
    $xfer = 0;
    $xfer += $output->writeStructBegin('server_get_seg_ids_result');
    if ($this->success !== null) {
      if (!is_array($this->success)) {
        throw new TProtocolException('Bad type in structure.', TProtocolException::INVALID_DATA);
      }
      $xfer += $output->writeFieldBegin('success', TType::LST, 0);
      {
        $output->writeListBegin(TType::I32, count($this->success));
        {
          foreach ($this->success as $iter20)
          {
            $xfer += $output->writeI32($iter20);
          }
        }
        $output->writeListEnd();
      }
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class server_compare_results_args {
  static $_TSPEC;

  public $old_results = null;
  public $new_result = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'old_results',
          'type' => TType::LST,
          'etype' => TType::STRUCT,
          'elem' => array(
            'type' => TType::STRUCT,
            'class' => 'result',
            ),
          ),
        2 => array(
          'var' => 'new_result',
          'type' => TType::STRUCT,
          'class' => 'result',
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['old_results'])) {
        $this->old_results = $vals['old_results'];
      }
      if (isset($vals['new_result'])) {
        $this->new_result = $vals['new_result'];
      }
    }
  }

  public function getName() {
    return 'server_compare_results_args';
  }

  public function read($input)
  {
    $xfer = 0;
    $fname = null;
    $ftype = 0;
    $fid = 0;
    $xfer += $input->readStructBegin($fname);
    while (true)
    {
      $xfer += $input->readFieldBegin($fname, $ftype, $fid);
      if ($ftype == TType::STOP) {
        break;
      }
      switch ($fid)
      {
        case 1:
          if ($ftype == TType::LST) {
            $this->old_results = array();
            $_size21 = 0;
            $_etype24 = 0;
            $xfer += $input->readListBegin($_etype24, $_size21);
            for ($_i25 = 0; $_i25 < $_size21; ++$_i25)
            {
              $elem26 = null;
              $elem26 = new result();
              $xfer += $elem26->read($input);
              $this->old_results []= $elem26;
            }
            $xfer += $input->readListEnd();
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 2:
          if ($ftype == TType::STRUCT) {
            $this->new_result = new result();
            $xfer += $this->new_result->read($input);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        default:
          $xfer += $input->skip($ftype);
          break;
      }
      $xfer += $input->readFieldEnd();
    }
    $xfer += $input->readStructEnd();
    return $xfer;
  }

  public function write($output) {
    $xfer = 0;
    $xfer += $output->writeStructBegin('server_compare_results_args');
    if ($this->old_results !== null) {
      if (!is_array($this->old_results)) {
        throw new TProtocolException('Bad type in structure.', TProtocolException::INVALID_DATA);
      }
      $xfer += $output->writeFieldBegin('old_results', TType::LST, 1);
      {
        $output->writeListBegin(TType::STRUCT, count($this->old_results));
        {
          foreach ($this->old_results as $iter27)
          {
            $xfer += $iter27->write($output);
          }
        }
        $output->writeListEnd();
      }
      $xfer += $output->writeFieldEnd();
    }
    if ($this->new_result !== null) {
      if (!is_object($this->new_result)) {
        throw new TProtocolException('Bad type in structure.', TProtocolException::INVALID_DATA);
      }
      $xfer += $output->writeFieldBegin('new_result', TType::STRUCT, 2);
      $xfer += $this->new_result->write($output);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class server_compare_results_result {
  static $_TSPEC;

  public $success = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        0 => array(
          'var' => 'success',
          'type' => TType::DOUBLE,
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['success'])) {
        $this->success = $vals['success'];
      }
    }
  }

  public function getName() {
    return 'server_compare_results_result';
  }

  public function read($input)
  {
    $xfer = 0;
    $fname = null;
    $ftype = 0;
    $fid = 0;
    $xfer += $input->readStructBegin($fname);
    while (true)
    {
      $xfer += $input->readFieldBegin($fname, $ftype, $fid);
      if ($ftype == TType::STOP) {
        break;
      }
      switch ($fid)
      {
        case 0:
          if ($ftype == TType::DOUBLE) {
            $xfer += $input->readDouble($this->success);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        default:
          $xfer += $input->skip($ftype);
          break;
      }
      $xfer += $input->readFieldEnd();
    }
    $xfer += $input->readStructEnd();
    return $xfer;
  }

  public function write($output) {
    $xfer = 0;
    $xfer += $output->writeStructBegin('server_compare_results_result');
    if ($this->success !== null) {
      $xfer += $output->writeFieldBegin('success', TType::DOUBLE, 0);
      $xfer += $output->writeDouble($this->success);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

?>
