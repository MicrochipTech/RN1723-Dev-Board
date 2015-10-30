<!DOCTYPE html>
<html lang="en">
    <head>
        %include('header.tpl')
    </head>
    <body>
        <!-- Include each module that makes up each page -->
        %include navbar

        <div class="container-fluid">
            <div class="row" id="header_image">
                <div class="col-sm-10 col-sm-offset-1 col-lg-8 col-lg-offset-2">
                    <img src="static/header_logo.png" class="img-responsive" alt="RN1723 Development Kit Server">
                </div>
            </div>
        </div>
        <!-- Load the rest of the page contents based on status of cookie -->


        % for item in eclient:
        <div class="container-fluid">
            <div class="row">
                <div class="col-sm-10 col-sm-offset-1 col-lg-8 col-lg-offset-2">
                    <div class="panel panel-default">
                        <div class="panel-heading">
                            <p class="panel-title">Device: <a href="/RN1723/App1/v1/{{item}}/control">{{eclient[item].uuid}}</a></p>
                        </div>
                        <div class="panel-body">
                            <div class="row">
                                <div class="col-xs-4 text-center">
                                    <p>Update Count: <b>{{eclient[item].URICount}}</b></p>
                                </div>
                                <div class="col-xs-4 text-center">
                                    <form action="/RN1723/App1/v1/{{item}}/resetURICounter" method="post">
                                    <p><input type="submit" name="Reset" value="Reset" onclick="return confirm('Are you sure?')"/></p>
                                    </form>
                                </div>
                                <div class="col-xs-4 text-center">
                                    <p>Total Update Count: <b>{{eclient[item].totalURICount}}</b></p>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
        % end

        <!-- Include the javascript for the page -->
        <script src="static/jquery-1.11.1.min.js"></script>
        <script src="static/bootstrap.min.js"></script>
    </body>

    <footer>
        %include('footer.tpl')
    </footer>
</html>
