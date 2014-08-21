<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Util;

class CAPxMapper {

  /**
   * Wrapper for capx_cfe_load_multiple(mappers)
   * @return [type] [description]
   */
  public static function loadAllMappers() {
    return capx_cfe_load_multiple(FALSE, array('type' => 'mapper'));
  }

  /**
   * Wrapper for capx_cfe_load_by_machine_name & capx_cfe_load
   * @param  [type] $key [description]
   * @return [type]      [description]
   */
  public static function loadMapper($key) {

    if (is_numeric($key)) {
      return capx_cfe_load_multiple($key, array('type' => 'mapper'));
    }
    else {
      return capx_cfe_load_by_machine_name($key, 'mapper');
    }

  }

}
