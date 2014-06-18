(function ($)
{
    $(document).ready(function() {

//        alert("You are running jQuery version: " + $.fn.jquery);

        // ensure that secondary affil checkbox matching primary is checked and disabled
        _ses_set_secondary_checkboxes($('#edit-field-primary-affiliations-und').val());

        // when the primary affil changes, check the matching secondary affil and disable it
        $('#edit-field-primary-affiliations-und').change(function() {
            _ses_set_secondary_checkboxes($(this).val());
        });

        console.log(Drupal.settings);
        console.log(Drupal.settings.allowAdminTab);
        console.log(Drupal.settings.allowAdditionalTab);
        if (!Drupal.settings.allowAdminTab) {
            $('.group-account-main li:last').hide();
        }
        if (!Drupal.settings.allowAdditionalTab) {
            $('.group-account-main li:last').prev().hide();
        }

    });

    function _ses_set_secondary_checkboxes(selVal) {
        // find the secondary affil checkbox that matches the value of the primary affil
        var findVal = 'input[name="field_secondary_affiliations[und][' + selVal + ']"]';
        var fieldName = '#edit-field-secondary-affiliations-und';
 
        // check the matching secondary affil checkbox, enable all checkboxes and disable matching
        $(fieldName).find(findVal).prop('checked','checked');
        $(fieldName).find('input[type=checkbox]:disabled').prop('disabled',false);
        $(fieldName).find(findVal).prop('disabled',true);
    }

}(jQuery));
