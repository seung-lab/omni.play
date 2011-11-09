<?php
/**
 * Autogenerated by Thrift Compiler (0.7.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 */
include_once $GLOBALS['THRIFT_ROOT'].'/Thrift.php';


$GLOBALS['E_viewType'] = array(
  'XY_VIEW' => 1,
  'XZ_VIEW' => 2,
  'ZY_VIEW' => 3,
);

final class viewType {
  const XY_VIEW = 1;
  const XZ_VIEW = 2;
  const ZY_VIEW = 3;
  static public $__names = array(
    1 => 'XY_VIEW',
    2 => 'XZ_VIEW',
    3 => 'ZY_VIEW',
  );
}

$GLOBALS['E_dataType'] = array(
  'INT8' => 1,
  'UINT8' => 2,
  'INT32' => 3,
  'UINT32' => 4,
  'FLOAT' => 5,
);

final class dataType {
  const INT8 = 1;
  const UINT8 = 2;
  const INT32 = 3;
  const UINT32 = 4;
  const FLOAT = 5;
  static public $__names = array(
    1 => 'INT8',
    2 => 'UINT8',
    3 => 'INT32',
    4 => 'UINT32',
    5 => 'FLOAT',
  );
}

class vector3d {
  static $_TSPEC;

  public $x = null;
  public $y = null;
  public $z = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'x',
          'type' => TType::DOUBLE,
          ),
        2 => array(
          'var' => 'y',
          'type' => TType::DOUBLE,
          ),
        3 => array(
          'var' => 'z',
          'type' => TType::DOUBLE,
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['x'])) {
        $this->x = $vals['x'];
      }
      if (isset($vals['y'])) {
        $this->y = $vals['y'];
      }
      if (isset($vals['z'])) {
        $this->z = $vals['z'];
      }
    }
  }

  public function getName() {
    return 'vector3d';
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
          if ($ftype == TType::DOUBLE) {
            $xfer += $input->readDouble($this->x);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 2:
          if ($ftype == TType::DOUBLE) {
            $xfer += $input->readDouble($this->y);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 3:
          if ($ftype == TType::DOUBLE) {
            $xfer += $input->readDouble($this->z);
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
    $xfer += $output->writeStructBegin('vector3d');
    if ($this->x !== null) {
      $xfer += $output->writeFieldBegin('x', TType::DOUBLE, 1);
      $xfer += $output->writeDouble($this->x);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->y !== null) {
      $xfer += $output->writeFieldBegin('y', TType::DOUBLE, 2);
      $xfer += $output->writeDouble($this->y);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->z !== null) {
      $xfer += $output->writeFieldBegin('z', TType::DOUBLE, 3);
      $xfer += $output->writeDouble($this->z);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class vector3i {
  static $_TSPEC;

  public $x = null;
  public $y = null;
  public $z = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'x',
          'type' => TType::I32,
          ),
        2 => array(
          'var' => 'y',
          'type' => TType::I32,
          ),
        3 => array(
          'var' => 'z',
          'type' => TType::I32,
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['x'])) {
        $this->x = $vals['x'];
      }
      if (isset($vals['y'])) {
        $this->y = $vals['y'];
      }
      if (isset($vals['z'])) {
        $this->z = $vals['z'];
      }
    }
  }

  public function getName() {
    return 'vector3i';
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
          if ($ftype == TType::I32) {
            $xfer += $input->readI32($this->x);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 2:
          if ($ftype == TType::I32) {
            $xfer += $input->readI32($this->y);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 3:
          if ($ftype == TType::I32) {
            $xfer += $input->readI32($this->z);
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
    $xfer += $output->writeStructBegin('vector3i');
    if ($this->x !== null) {
      $xfer += $output->writeFieldBegin('x', TType::I32, 1);
      $xfer += $output->writeI32($this->x);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->y !== null) {
      $xfer += $output->writeFieldBegin('y', TType::I32, 2);
      $xfer += $output->writeI32($this->y);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->z !== null) {
      $xfer += $output->writeFieldBegin('z', TType::I32, 3);
      $xfer += $output->writeI32($this->z);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class bbox {
  static $_TSPEC;

  public $min = null;
  public $max = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'min',
          'type' => TType::STRUCT,
          'class' => 'vector3d',
          ),
        2 => array(
          'var' => 'max',
          'type' => TType::STRUCT,
          'class' => 'vector3d',
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['min'])) {
        $this->min = $vals['min'];
      }
      if (isset($vals['max'])) {
        $this->max = $vals['max'];
      }
    }
  }

  public function getName() {
    return 'bbox';
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
            $this->min = new vector3d();
            $xfer += $this->min->read($input);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 2:
          if ($ftype == TType::STRUCT) {
            $this->max = new vector3d();
            $xfer += $this->max->read($input);
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
    $xfer += $output->writeStructBegin('bbox');
    if ($this->min !== null) {
      if (!is_object($this->min)) {
        throw new TProtocolException('Bad type in structure.', TProtocolException::INVALID_DATA);
      }
      $xfer += $output->writeFieldBegin('min', TType::STRUCT, 1);
      $xfer += $this->min->write($output);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->max !== null) {
      if (!is_object($this->max)) {
        throw new TProtocolException('Bad type in structure.', TProtocolException::INVALID_DATA);
      }
      $xfer += $output->writeFieldBegin('max', TType::STRUCT, 2);
      $xfer += $this->max->write($output);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class tile {
  static $_TSPEC;

  public $view = null;
  public $bounds = null;
  public $data = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'view',
          'type' => TType::I32,
          ),
        2 => array(
          'var' => 'bounds',
          'type' => TType::STRUCT,
          'class' => 'bbox',
          ),
        3 => array(
          'var' => 'data',
          'type' => TType::STRING,
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['view'])) {
        $this->view = $vals['view'];
      }
      if (isset($vals['bounds'])) {
        $this->bounds = $vals['bounds'];
      }
      if (isset($vals['data'])) {
        $this->data = $vals['data'];
      }
    }
  }

  public function getName() {
    return 'tile';
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
          if ($ftype == TType::I32) {
            $xfer += $input->readI32($this->view);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 2:
          if ($ftype == TType::STRUCT) {
            $this->bounds = new bbox();
            $xfer += $this->bounds->read($input);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 3:
          if ($ftype == TType::STRING) {
            $xfer += $input->readString($this->data);
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
    $xfer += $output->writeStructBegin('tile');
    if ($this->view !== null) {
      $xfer += $output->writeFieldBegin('view', TType::I32, 1);
      $xfer += $output->writeI32($this->view);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->bounds !== null) {
      if (!is_object($this->bounds)) {
        throw new TProtocolException('Bad type in structure.', TProtocolException::INVALID_DATA);
      }
      $xfer += $output->writeFieldBegin('bounds', TType::STRUCT, 2);
      $xfer += $this->bounds->write($output);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->data !== null) {
      $xfer += $output->writeFieldBegin('data', TType::STRING, 3);
      $xfer += $output->writeString($this->data);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class result {
  static $_TSPEC;

  public $selected = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'selected',
          'type' => TType::LST,
          'etype' => TType::I32,
          'elem' => array(
            'type' => TType::I32,
            ),
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['selected'])) {
        $this->selected = $vals['selected'];
      }
    }
  }

  public function getName() {
    return 'result';
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
            $this->selected = array();
            $_size0 = 0;
            $_etype3 = 0;
            $xfer += $input->readListBegin($_etype3, $_size0);
            for ($_i4 = 0; $_i4 < $_size0; ++$_i4)
            {
              $elem5 = null;
              $xfer += $input->readI32($elem5);
              $this->selected []= $elem5;
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
    $xfer += $output->writeStructBegin('result');
    if ($this->selected !== null) {
      if (!is_array($this->selected)) {
        throw new TProtocolException('Bad type in structure.', TProtocolException::INVALID_DATA);
      }
      $xfer += $output->writeFieldBegin('selected', TType::LST, 1);
      {
        $output->writeListBegin(TType::I32, count($this->selected));
        {
          foreach ($this->selected as $iter6)
          {
            $xfer += $output->writeI32($iter6);
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

class metadata {
  static $_TSPEC;

  public $uri = null;
  public $bounds = null;
  public $resolution = null;
  public $type = null;
  public $chunkDims = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'uri',
          'type' => TType::STRING,
          ),
        2 => array(
          'var' => 'bounds',
          'type' => TType::STRUCT,
          'class' => 'bbox',
          ),
        3 => array(
          'var' => 'resolution',
          'type' => TType::STRUCT,
          'class' => 'vector3i',
          ),
        4 => array(
          'var' => 'type',
          'type' => TType::I32,
          ),
        5 => array(
          'var' => 'chunkDims',
          'type' => TType::STRUCT,
          'class' => 'vector3i',
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['uri'])) {
        $this->uri = $vals['uri'];
      }
      if (isset($vals['bounds'])) {
        $this->bounds = $vals['bounds'];
      }
      if (isset($vals['resolution'])) {
        $this->resolution = $vals['resolution'];
      }
      if (isset($vals['type'])) {
        $this->type = $vals['type'];
      }
      if (isset($vals['chunkDims'])) {
        $this->chunkDims = $vals['chunkDims'];
      }
    }
  }

  public function getName() {
    return 'metadata';
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
          if ($ftype == TType::STRING) {
            $xfer += $input->readString($this->uri);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 2:
          if ($ftype == TType::STRUCT) {
            $this->bounds = new bbox();
            $xfer += $this->bounds->read($input);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 3:
          if ($ftype == TType::STRUCT) {
            $this->resolution = new vector3i();
            $xfer += $this->resolution->read($input);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 4:
          if ($ftype == TType::I32) {
            $xfer += $input->readI32($this->type);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 5:
          if ($ftype == TType::STRUCT) {
            $this->chunkDims = new vector3i();
            $xfer += $this->chunkDims->read($input);
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
    $xfer += $output->writeStructBegin('metadata');
    if ($this->uri !== null) {
      $xfer += $output->writeFieldBegin('uri', TType::STRING, 1);
      $xfer += $output->writeString($this->uri);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->bounds !== null) {
      if (!is_object($this->bounds)) {
        throw new TProtocolException('Bad type in structure.', TProtocolException::INVALID_DATA);
      }
      $xfer += $output->writeFieldBegin('bounds', TType::STRUCT, 2);
      $xfer += $this->bounds->write($output);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->resolution !== null) {
      if (!is_object($this->resolution)) {
        throw new TProtocolException('Bad type in structure.', TProtocolException::INVALID_DATA);
      }
      $xfer += $output->writeFieldBegin('resolution', TType::STRUCT, 3);
      $xfer += $this->resolution->write($output);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->type !== null) {
      $xfer += $output->writeFieldBegin('type', TType::I32, 4);
      $xfer += $output->writeI32($this->type);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->chunkDims !== null) {
      if (!is_object($this->chunkDims)) {
        throw new TProtocolException('Bad type in structure.', TProtocolException::INVALID_DATA);
      }
      $xfer += $output->writeFieldBegin('chunkDims', TType::STRUCT, 5);
      $xfer += $this->chunkDims->write($output);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

?>
