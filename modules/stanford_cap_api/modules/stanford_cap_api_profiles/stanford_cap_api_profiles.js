(function ($) {
  Drupal.behaviors.cap_profiles_fields = {
    attach: function (context, settings) {
      $('#fields input', context).click(function () {
        var parent_path = $(this).attr('parent');
        var checked = $(this).attr('checked');
        // Field selected.
        if (checked && parent_path.length !== 0) {
          parent_set_checked(parent_path, context);
        }
        else {
          var path = $(this).attr('value');
          child_unset_checked(path, context);
        }
      });

      function parent_set_checked(parent_path, context) {
        var $parent = $('#fields input[value="' + parent_path + '"]', context);
        $parent.attr('checked', true);
        var parent_path = $parent.attr('parent');
        if (parent_path.length !== 0) {
          parent_set_checked(parent_path, context)
        }
      }

      function child_unset_checked(path, context) {
        $('#fields input[parent="' + path + '"]', context).each(function (index, elem) {
          $(elem).attr('checked', false);
          child_unset_checked($(elem).attr('value'), context);
        });
      }
    }
  };
})(jQuery);
