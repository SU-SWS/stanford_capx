<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Util;
use CAPx\Drupal\Mapper\EntityMapper;

class CAPxMapper {

  /**
   * Wrapper for capx_cfe_load_multiple(mappers).
   *
   * @return array
   *   An array mapper objects
   */
  public static function loadAllMappers() {
    return capx_cfe_load_multiple(FALSE, array('type' => 'mapper'));
  }

  /**
   * Wrapper for capx_cfe_load_by_machine_name & capx_cfe_load.
   *
   * @param mixed $key
   * int - cfid
   * string - machine_name
   *
   * @return array
   *   A single mapper object in an array.
   */
  public static function loadMapper($key) {

    if (is_numeric($key)) {
      return capx_cfe_load_multiple($key, array('type' => 'mapper'));
    }
    else {
      return capx_cfe_load_by_machine_name($key, 'mapper');
    }

  }

  /**
   * Loads an EntityMapper instance by id or machine name.
   *
   * @param mixed $key
   *   machine name or id.
   *
   * @return EntityMapper
   *   A fully loaded entity Mapper instance.
   */
  public static function loadEntityMapper($key) {

    $mapperConfig = self::loadMapper($key);
    $mapper = new EntityMapper($mapperConfig);

    return $mapper;
  }

}
