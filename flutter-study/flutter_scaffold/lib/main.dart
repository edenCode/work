import 'package:flutter/material.dart';

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {

  @override
  Widget build(BuildContext context) {
    // TODO: implement build
    return  MaterialApp(
      home: ScaffoldRoute(),
    );
  }
}


// scaffold 脚手架
class ScaffoldRoute extends StatefulWidget{
  @override
  State<StatefulWidget> createState() => ScaffoldRouteState();
}



class ScaffoldRouteState extends State<ScaffoldRoute> with SingleTickerProviderStateMixin{

  static GlobalKey<ScaffoldState> _globalKey = new GlobalKey<ScaffoldState>();

  List<String> _tabs = ["首页","会员","通知","我的"];

  TabController _tabController;//= TabController(length: null, vsync: null)

  @override
  void initState() {
    // TODO: implement initState
    super.initState();
    _tabController = TabController(length: _tabs.length, vsync: this);
  }

    @override
  Widget build(BuildContext context) {
    // TODO: implement build
    return Scaffold(
      key: _globalKey,
      appBar: AppBar( // 导航栏
        title: Text("Test Scaffold"),
        centerTitle: true,
        leading: Builder( // appbar 左边 按钮
          builder: (context){
              //_globalKey.currentState.openDrawer();
              //Scaffold.of(context).openDrawer();
              return IconButton(
                icon: Icon(Icons.dashboard),
                onPressed: ()=>  _globalKey.currentState.openDrawer(),//Scaffold.of(context).openDrawer(),
              );
            },
        ),
        actions: <Widget>[
          IconButton(icon: Icon(Icons.share), onPressed: (){}),
        ],
      ),
      drawer: MyDrawer(),
      bottomNavigationBar: TabBar(
          controller: _tabController,
          tabs: _tabs.map((e){
              return Tab(icon:Text(e,style: TextStyle(color: Colors.red,fontSize: 15),),);//Text(e,style: TextStyle(fontSize: 18),textDirection: TextDirection.ltr,);//Tab(text: e,);
          }).toList(),
      ),
      body: TabBarView(
          controller: _tabController,
          children: _tabs.map((e){
          return Container(
            color: Colors.white,
            alignment: Alignment.center,
            child: Text(e),
          );
      }).toList(),
      ),
    );
  }
}

/// 侧滑菜单栏
///
class MyDrawer extends StatelessWidget{
  @override
  Widget build(BuildContext context) {
    // TODO: implement build
    return Drawer(
      child: MediaQuery.removePadding(
          context:context,
          removeTop: true,
          child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: <Widget>[
                Padding(
                  padding: EdgeInsets.all(38.0),
                  child: Row(
                    crossAxisAlignment:CrossAxisAlignment.start,
                    verticalDirection: VerticalDirection.up,
                    children: <Widget>[
                      Padding(
                        padding: EdgeInsets.symmetric(horizontal: 16.0),
                        child: ClipOval(
                          child: Image.asset(
                              "images/account_circle.png",
                            width: 80,
                          ),
                        ),
                      ),
                      Container(
                        //constraints: BoxConstraints.expand(),

                        alignment: Alignment.center,
                        child: Text("gongcm",style: TextStyle(fontSize: 18),),
                      )
                      //Text("gongcm",style: TextStyle(fontSize: 18),)
                    ],
                  ),
                ),
                Expanded(
                  child: ListView(
                    children: <Widget>[
                      ListTile(
                        leading: Icon(Icons.add),
                        title: Text("add"),
                      )
                      ,
                      ListTile(
                        leading: Icon(Icons.settings),
                        title: Text("setting"),
                      ),

                    ],
                  ),
                )
              ],
          )
      ),
    );
  }
}



//class MyTab extends