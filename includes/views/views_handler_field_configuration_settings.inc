<?php

/**
 * @file
 * Definition of stanford_capx_handler_field_configuration_settings.
 */

use CAPx\Drupal\Util\CAPxMapper;

/**
 * Field handler to show data of serialized fields and also generate a link
 * to the related Mapper for an Importer.
 *
 * @ingroup views_field_handlers
 */
class views_handler_field_configuration_settings extends views_handler_field_serialized {

  function option_definition() {
    $options = parent::option_definition();
    $options['mapper'] = array('default' => FALSE);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['format']['#options']['mapper'] = t('Mapper link');
    $form['mapper'] = array(
      '#type' => 'checkbox',
      '#title' => t('Render as plain text'),
      '#default_value' => $this->options['mapper'],
      '#dependency' => array('edit-options-format' => array('mapper')),
    );
  }

  function render($values) {

    if ($this->options['format'] == 'mapper') {
      $value = $values->{$this->field_alias};
      $field_values = unserialize($value);

      if (!empty($field_values['mapper'])) {
        $mapper = CAPxMapper::loadMapper($field_values['mapper']);
        if (isset($mapper)) {
          if ($this->options['mapper']) {
            return check_plain($mapper->title);
          }
          else {
            return l(check_plain($mapper->title), 'admin/config/capx/mapper/edit/' . $mapper->identifier());
          }
        }
      }
      else {
        return t('Mapper not found');
      }
    }

    return parent::render($values);
  }
}
