(function($, undefined) {
    var timeout;

    function test() {
        var $tbody = $("#matches tbody"),
            $group = $("#group");;

        $.post("/regex", JSON.stringify({
            regex: $("#regex").val(),
            text: $("#text").val(),
            search: true
        })).always(function() {
            $("#matches tbody > tr").remove();
            $group.removeClass("has-error has-success");
        }).done(function(res) {
            $group.addClass("has-success");
            $.each(res.matches, function(index, value) {
                $tbody.append("<tr><td>" + index + "</td><td>" + value + "</td></tr>");
            });
        }).fail(function(info) {
            if (info.status === 400) {
                $group.addClass("has-error");
                $tbody.append("<tr><td colspan=\"2\">Invalid regex</td></tr>");
            }
        });
    }

    function onInput() {
        if (timeout) clearTimeout(timeout);
        timeout = setTimeout(test, 500);
    }

    $("#regex").input(onInput);
    $("#text").input(onInput);

    $("#form").submit(function() { return false; });
})(jQuery);
