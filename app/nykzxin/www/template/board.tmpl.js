<table data-role="table" id="table_board" data-mode="columntoggle" class="ui-responsive table-stroke">
  <thead>
    <tr>
      <th data-priority="2">序号</th>
      <th data-priority="4">状态</th>
      <th data-priority="1">作者</th>
      <th data-priority="3">日期</th>
      <th data-priority="persist"><b>标题</b></th>
    </tr>
  </thead>
  <tbody>
    <% _.each(posts,function(post,key){ %>
    <tr>
      <td><%= key + start %></td>
      <td><%= post.level ? post.level : ""  %></td>
      <td><a href="#" onclick='gotoUser(<%= post.owner %>")'><%= post.owner %></a></td>
      <td><%= new Date(post.id*1000).toLocaleDateString() + "<br/>" + new Date(post.id*1000).toLocaleTimeString() %></td>
      <td><a href="#" onclick='gotoPost("<%= board %>","<%= post.filename %>","<%= returnpageid %>")'><%= post.title %></a></td>
      </tr>
    <% }); %>
  </tbody>
</table>
