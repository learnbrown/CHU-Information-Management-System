/*
 * Project Name: Information Management System
 * Author: ReinerBrown
 * Copyright (c) 2024 ReinerBrown
 * License: MIT License
 *
 * This project uses the following third-party libraries:
 *
 * 1. Crow Framework
 *    Copyright (c) 2014-2017, ipkn
 *                     2020-2022, CrowCpp
 *    Licensed under the BSD 3-Clause License.
 *
 * 2. nlohmann/json Library
 *    Copyright (c) 2013-2023, Niels Lohmann
 *    Licensed under the MIT License.
 */

#include "crow.h"
#include <sqlite3.h>
#include <iostream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using namespace std;

int main() {
	crow::SimpleApp app;

	// 初始化SQLite数据库
	sqlite3* db;
	int rc = sqlite3_open("info.db", &db);
	if (rc) {
		cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
		return 1;
	}

	
	// 登录函数
	CROW_ROUTE(app, "/login").methods("POST"_method)([db](const crow::request& req) {

		// 将请求体加载为json到body变量
		auto body = crow::json::load(req.body);

		if (!body) {
			return crow::response(401, "Invalid request body");
		}

		// 从json中获取请求的类型，可以是studnet、teacher、admin
		string user_type = body["user_type"].s();

		// student和teacher类型的登录
		if(user_type == "student" || user_type == "teacher") {

			// 从json中获取登录的账号密码
			int input_id = body["name"].i();
			int input_pwd = body["password"].i();


			// 查询的sql语句
			string sql = "SELECT * FROM " + user_type + "s WHERE id = ?;";

			/*
			sqlite3_stmt* stmt 是 SQLite 数据库 C API 中用于执行 SQL 查询的指针。它表示一个预处理 SQL 语句（prepared statement），通过这个指针可以执行 SQL 语句、绑定参数、获取查询结果等操作。

			预处理语句的作用：
			SQLite 使用预处理语句来提高查询效率并防止 SQL 注入攻击。预处理语句首先由 SQLite 编译，然后再执行。这样可以避免每次执行时都进行 SQL 编译，减少了重复工作。

			使用 sqlite3_stmt* stmt 的常见步骤：
			准备 SQL 语句：使用 sqlite3_prepare_v2 函数将 SQL 查询编译为预处理语句，并返回一个 sqlite3_stmt* 指针。
			绑定参数：使用 sqlite3_bind_* 系列函数将数据绑定到 SQL 语句中的占位符（如 ?）。
			执行 SQL 语句：通过 sqlite3_step 来执行查询。
			处理结果：查询成功时，使用 sqlite3_column_* 系列函数获取查询结果。
			清理资源：执行完成后，使用 sqlite3_finalize 释放资源。
			*/
			sqlite3_stmt* stmt;
			
			// 预处理sql语句
			int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
			if (rc != SQLITE_OK) {
				cerr << "SQL error" << endl;
				return crow::response(500, "Database error");
			}

			string error; // 存储错误信息
			
			// 将sql语句中的占位符(?)链接到变量
			sqlite3_bind_int(stmt, 1, input_id); 
			
			// json类型的对象，用于返回登录用户的信息，使用起来就类似于python的字典
			// user_info -> ["name":"admin", "password":"admin"] 
			// 他保存的就是这样一对一对的数据
			crow::json::wvalue user_info; 
			
			// 执行sql语句进行查询
			if(sqlite3_step(stmt) == SQLITE_ROW) {

				// 从查询的行中读取第0、第3列作为用户的账号密码
				// 这个函数最后一个下划线后跟的是数据库中列的类型，这里第0、第3列都是int型
				// 下划线后就跟int，我们也用int型变量去接受返回值
				// 第一个参数填的是 stmt 这个东西，反正每次都是输他
				// 第二个参数填的是列号，从0开始数的，函数返回的就是该条记录的那一列的数据
				int user_id = sqlite3_column_int(stmt, 0);
				int user_pwd = sqlite3_column_int(stmt, 3);

				// 根据不同的用户类型保存不同的信息
				if(user_type == "student") {

					// sqlite_column_text返回的字符串是 const unsigned char* 类型的，不能直接转为string
					// 因此用 reinterpret_cast<const char*>() 转换一下
					string stu_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
					string stu_course1 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
					string stu_course2 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
					int stu_score1 = sqlite3_column_int(stmt, 6);
					int stu_score2 = sqlite3_column_int(stmt, 7);
					string stu_phone_number = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
					int stu_gender = sqlite3_column_int(stmt, 9);
					string stu_wish = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10));

					// 将登录用户的信息打包到json
					user_info["id"] = user_id;
					user_info["name"] = stu_name;
					user_info["class"] = sqlite3_column_int(stmt, 2);
					user_info["course1"] = stu_course1;
					user_info["course2"] = stu_course2;
					user_info["score1"] = stu_score1;
					user_info["score2"] = stu_score2;
					user_info["phone_number"] = stu_phone_number;
					user_info["gender"] = stu_gender;
					user_info["wish"] = stu_wish;
				}else {
					string tecr_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
					string tecr_course_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
					string tecr_course_num1 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
					string tecr_course_num2 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

					user_info["id"] = user_id;
					user_info["name"] = tecr_name;
					user_info["course_num1"] = tecr_course_num1;
					user_info["course_num2"] = tecr_course_num2;
					user_info["course_name"] = tecr_course_name;
				}

				if(input_pwd != user_pwd)
					// 设置错误信息
					error = "Incorrect password";
			}else
				error = "Incorrect username";

			sqlite3_finalize(stmt); // 释放stmt

			// 如果有报错信息，就返回错误
			if(error.size()) {				
				return crow::response(401, error);
			}

			crow::response res;

			// 在cookie中保存id和type来记录登录状态
			res.add_header("Set-Cookie", "session_id="+ to_string(input_id) + ",session_type=" + user_type + "; HttpOnly; Path=/;");
			
			// 将json中的内容存入响应体中
			res.body = user_info.dump();

			return res;
		}

		// admin类型的登录
		else if (user_type == "admin") {
			auto body = crow::json::load(req.body);
			string name = body["name"].s();
			string pwd = body["password"].s();

			if(name == "admin" && pwd == "admin") {
				crow::response res;
				res.add_header("Set-cookie", "session_id=admin, session_type=admin; HttpOnly; Path=/;");

				string sql_stu = "SELECT * FROM requests_student;";
				string sql_tea = "SELECT * FROM requests_teacher;";

				sqlite3_stmt* stmt_stu;
				sqlite3_stmt* stmt_tea;

				int rc = sqlite3_prepare_v2(db, sql_stu.c_str(), -1, &stmt_stu, nullptr);
				if(rc != SQLITE_OK) {
					cerr << "SQL1 Error" << endl;
					cerr << sqlite3_errmsg(db) << endl;
				}
				rc = sqlite3_prepare_v2(db, sql_tea.c_str(), -1, &stmt_tea, nullptr);
				if(rc != SQLITE_OK) {
					cerr << "SQL2 Error" << endl;
				}

				vector<crow::json::wvalue> vec_stu;
				vector<crow::json::wvalue> vec_tea;

				while(sqlite3_step(stmt_stu) == SQLITE_ROW) {
					string req_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt_stu, 0));
					int id = sqlite3_column_int(stmt_stu, 1);
					string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt_stu, 2));
					int gender = sqlite3_column_int(stmt_stu, 3);
					string phone_number = reinterpret_cast<const char*>(sqlite3_column_text(stmt_stu, 4));
					string wish = reinterpret_cast<const char*>(sqlite3_column_text(stmt_stu, 5));

					crow::json::wvalue temp;
					temp["req_id"] = req_id;
					temp["id"] = id;
					temp["name"] = name;
					temp["gender"] = gender;
					temp["phone_number"] = phone_number;
					temp["wish"] = wish;

					vec_stu.push_back(temp);
				}

				while(sqlite3_step(stmt_tea) == SQLITE_ROW) {
					string req_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt_tea, 0));
					int stu_id = sqlite3_column_int(stmt_tea, 1);
					string option = reinterpret_cast<const char*>(sqlite3_column_text(stmt_tea, 2));
					int new_score = sqlite3_column_int(stmt_tea, 3);

					crow::json::wvalue temp;
					temp["req_id"] = req_id;
					temp["stu_id"] = stu_id;
					temp["option"] = option;
					temp["new_score"] = new_score;

					vec_tea.push_back(temp);
				}

				crow::json::wvalue admin;
				admin["students"] = move(vec_stu);
				admin["teachers"] = move(vec_tea);
				res.body = admin.dump();

				sqlite3_finalize(stmt_stu);
				sqlite3_finalize(stmt_tea);
				
				return res;
			}else
				return crow::response(401, " You\'re not the administrator");
		}


		return crow::response(401, "Default");
	});

	CROW_ROUTE(app, "/get_course").methods("POST"_method)([db](const crow::request& req) {
		auto cookie = req.get_header_value("Cookie");

		if(cookie.size() && cookie.find("session_id") != string::npos) {
			auto body = crow::json::load(req.body);
			
			string course_id = body["course_id"].s();

			string sql = "SELECT * FROM students WHERE course1 = ? or course2 = ?;";
			sqlite3_stmt* stmt;

			int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
			if(rc != SQLITE_OK) {
				return crow::response(401, "Database erroe");
			}

			sqlite3_bind_text(stmt, 1, course_id.c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_text(stmt, 2, course_id.c_str(), -1, SQLITE_TRANSIENT);
			
			vector<crow::json::wvalue> student_list;
			int cnt = 0;
			while(sqlite3_step(stmt) == SQLITE_ROW) {
				int stu_id = sqlite3_column_int(stmt, 0);
				string stu_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
				int stu_class = sqlite3_column_int(stmt, 2);
				string stu_crs1 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
				string stu_crs2 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
				int score1 = sqlite3_column_int(stmt, 6);
				int score2 = sqlite3_column_int(stmt, 7);
				bool able1 = sqlite3_column_int(stmt, 11);
				bool able2 = sqlite3_column_int(stmt, 12);

				crow::json::wvalue temp;

				temp["id"] = stu_id;
				temp["name"] = stu_name;
				temp["class"] = stu_class;
				temp["score"] = (stu_crs1 == course_id ? score1 : score2);
				temp["able"] = (stu_crs1 == course_id ? able1 : able2);
				student_list.push_back(temp);
			}
			crow::json::wvalue students = move(student_list);

			crow::response res;
			res.body = students.dump();

			return res;
		}


		
		return crow::response(401, "Please login first");
	});
	
	CROW_ROUTE(app, "/insert_score").methods("POST"_method)([db](const crow::request& req) {
		auto body = nlohmann::json::parse(req.body);

		if(!body.is_array()) {
			return crow::response(400, "Invalid JSON format, expected an array.");
		}

		for(const auto& student : body) {
			int stu_id = student["stu_id"];
			string option = student["option"];
			int new_score = student["new_score"];

			string able = option == "score1"? "able_to_revise1" : "able_to_revise2";

			string sql = "UPDATE students SET " + option + " = ?, " + able + " = 0 WHERE id = ?";
			sqlite3_stmt* stmt;

			int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
			if(rc != SQLITE_OK) {
				cerr << "SQL Error:" << sqlite3_errmsg(db);
				return crow::response(401, "SQL Error");
			}

			sqlite3_bind_int(stmt, 1, new_score);
			sqlite3_bind_int(stmt, 2, stu_id);

			if(sqlite3_step(stmt) != SQLITE_DONE) {
				cerr << "Failed to insert" << endl;
				return crow::response(401, "Failed to insert");
			}

			sqlite3_finalize(stmt);
		}

		return crow::response(200, "Successfully");
	});

	CROW_ROUTE(app, "/revise_score").methods("POST"_method)([db](const crow::request& req) {
		auto body = crow::json::load(req.body);

		string req_id = body["req_time"].s();
		int stu_id = body["stu_id"].i();
		string option = body["option"].s();
		int new_score = body["new_score"].i();

		// string able = option == "score1"? "able1" : "able2";


		string insert_sql = R"(
			INSERT INTO requests_teacher (req_id, stu_id, option, new_score)
			VALUES (?, ?, ?, ?)
		)";
		
		sqlite3_stmt* insert_stmt;

		int rc = sqlite3_prepare_v2(db, insert_sql.c_str(), -1, &insert_stmt, nullptr);
		if(rc != SQLITE_OK){
			cerr << "INSERT SQL Error" << endl;
			return crow::response(401, "SQL ERROR");
		}

		sqlite3_bind_text(insert_stmt, 1, req_id.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(insert_stmt, 2, stu_id);
		sqlite3_bind_text(insert_stmt, 3, option.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(insert_stmt, 4, new_score);

		if(sqlite3_step(insert_stmt) != SQLITE_DONE) {
			return crow::response(401, "Failed to revise" , sqlite3_errmsg(db));
		}

		return crow::response(200, "Successfully");
	});

	//处理学生和老师发送过来的请求
	CROW_ROUTE(app, "/unsolvereq").methods("POST"_method)([db](const crow::request& req){
		// 将请求体加载为json到body变量
		auto body = crow::json::load(req.body);

		if (!body) {
			return crow::response(400, "Invalid request body");
		}

		// 从body中获取req_status, req_id和req_type
		std::string req_status = body["req_status"].s();
		std::string req_id = body["req_id"].s();
		std::string req_type = body["req_type"].s();

		// 根据req_status的值执行不同的操作
		if (req_status == "确认") {
			//老师请求
			if (req_type=="teacher") {
		
				// 查询的sql语句
				string sql = "SELECT * FROM requests_teacher WHERE req_id = \"" + req_id + "\";";

				sqlite3_stmt* stmt;
				
				string error;

				// 预处理sql语句
				int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
				if (rc != SQLITE_OK) {
					cerr << "SQL error" << endl;
					error="Database error";
				}
		
				// 将sql语句中的占位符(?)链接到变量
				sqlite3_bind_text(stmt, 1, req_id.c_str(), -1, SQLITE_STATIC);
			
				// 执行sql语句进行查询
				if(sqlite3_step(stmt) != SQLITE_ROW) {
					cerr << sqlite3_errmsg(db) << endl;
				}

				int stu_id = sqlite3_column_int(stmt, 1);
				string score = reinterpret_cast<const char*>(sqlite3_column_text(stmt,2));
				int aft_score = sqlite3_column_int(stmt,3);
				
				//将students表单中id为stu_id的score修改为aft_score
				std::string update_sql = "UPDATE students SET " + score + "= ? WHERE id = ?;";

				sqlite3_stmt* update_stmt;
				rc = sqlite3_prepare_v2(db, update_sql.c_str(), -1, &update_stmt, nullptr);
				if (rc != SQLITE_OK) {
					std::cerr << "Update SQL error: " << sqlite3_errmsg(db) << std::endl;
					error="Database error";
				}
				// 绑定新分数和学生ID到更新语句
				sqlite3_bind_int(update_stmt, 1, aft_score);
				sqlite3_bind_int(update_stmt, 2, stu_id);

				// 执行更新语句
				if (sqlite3_step(update_stmt) != SQLITE_DONE) {
					std::cerr << "Update error: " << sqlite3_errmsg(db) << std::endl;
					sqlite3_finalize(update_stmt);
					error= "Database error";
				}

				// 完成更新后，需要重置update_stmt
				sqlite3_reset(update_stmt);
				sqlite3_finalize(update_stmt);

				//销毁
				sqlite3_finalize(stmt);

				string delete_sql = "DELETE FROM requests_teacher WHERE req_id = \"" + req_id +"\" ;";
				sqlite3_stmt* delete_stmt;

				rc = sqlite3_prepare_v2(db, delete_sql.c_str(), -1, &delete_stmt, nullptr);
				if(rc != SQLITE_OK) {
					cerr << "DELETE SQL error: " << sqlite3_errmsg(db) << endl;
					return crow::response(401, "DELETE SQL error");
				}

				if(sqlite3_step(delete_stmt) != SQLITE_DONE) {
					cerr << "DELETE error: " << sqlite3_errmsg(db) << endl;
					return crow::response(401, "DELETE error");
				}

				// 如果有报错信息，就返回错误
				if(error.size()) {				
					return crow::response(401, error);
				}
				return crow::response(200, "Update successful");
			}
			//学生请求
			if (req_type=="student") {
		
				// 查询的sql语句
				string sql = "SELECT * FROM requests_student WHERE req_id = \"" + req_id + "\";";

				sqlite3_stmt* stmt;

				// 预处理sql语句
				int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
				if (rc != SQLITE_OK) {
					cerr << "SQL error" << endl;
					cerr << sqlite3_errmsg(db) << endl;
					return crow::response(500, "Database error");
				}
		
				// 将sql语句中的占位符(?)链接到变量
				sqlite3_bind_text(stmt, 1, req_id.c_str(), -1, SQLITE_STATIC);
			
				// 执行sql语句进行查询
				if(sqlite3_step(stmt) != SQLITE_ROW) {
					cerr << sqlite3_errmsg(db) << endl;
				}

				int stu_id = sqlite3_column_int(stmt, 1);
				int gender = sqlite3_column_int(stmt,3);
				string phone_num = reinterpret_cast<const char*>(sqlite3_column_text(stmt,4));
				string wish = reinterpret_cast<const char*>(sqlite3_column_text(stmt,5));
				//将students表单中id为stu_id的gender、phone_num、wish修改
				std::string update_sql = "UPDATE students SET gender = ?, phone_number = ?,  wish = ? WHERE id = ?;";

				sqlite3_stmt* update_stmt;
				rc = sqlite3_prepare_v2(db, update_sql.c_str(), -1, &update_stmt, nullptr);
				if (rc != SQLITE_OK) {
					std::cerr << "Update SQL error: " << sqlite3_errmsg(db) << std::endl;
					sqlite3_finalize(stmt);
					return crow::response(500, "Database error");
				}
					// 将更新语句中的占位符(?)绑定到变量
				sqlite3_bind_int(update_stmt, 1, gender);
				sqlite3_bind_text(update_stmt, 2, phone_num.c_str(), -1, SQLITE_STATIC);
				sqlite3_bind_text(update_stmt, 3, wish.c_str(), -1, SQLITE_STATIC);
				sqlite3_bind_int(update_stmt, 4, stu_id);

					// 执行更新语句
				if (sqlite3_step(update_stmt) != SQLITE_DONE) {
					std::cerr << "Update error: " << sqlite3_errmsg(db) << std::endl;
					sqlite3_finalize(stmt);
					sqlite3_finalize(update_stmt);
					return crow::response(500, "Database error");
				}
				// 完成更新后，需要重置和销毁update_stmt
				sqlite3_finalize(update_stmt);
				// 完成查询后，需要销毁stmt
				sqlite3_finalize(stmt);


				string delete_sql = "DELETE FROM requests_student WHERE req_id = \"" + req_id +"\" ;";
				sqlite3_stmt* delete_stmt;

				rc = sqlite3_prepare_v2(db, delete_sql.c_str(), -1, &delete_stmt, nullptr);
				if(rc != SQLITE_OK) {
					cerr << "DELETE SQL error: " << sqlite3_errmsg(db) << endl;
					return crow::response(401, "DELETE SQL error");
				}

				if(sqlite3_step(delete_stmt) != SQLITE_DONE) {
					cerr << "DELETE error: " << sqlite3_errmsg(db) << endl;
					return crow::response(401, "DELETE error");
				}

				// 返回成功响应
				return crow::response(200, "Update successful");
			}
		} else if (req_status == "取消") {
			if (req_type=="teacher") {
				// 查询的sql语句，查找到req_id这条记录
				string sql = "SELECT * FROM requests_teacher WHERE req_id = \"" + req_id + "\";";

				sqlite3_stmt* stmt;
				string error;
				// 预处理sql语句
				int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
				if (rc != SQLITE_OK) {
					cerr << "SQL error" << endl;
					error="Database error";
				}
		
				//查找到记录后，将这条记录删除
				if (sqlite3_step(stmt) == SQLITE_ROW) {
					// 找到了记录，现在执行删除操作
					std::string delete_sql = "DELETE FROM requests_teacher WHERE req_id = \"" + req_id+ "\";";

					sqlite3_stmt* delete_stmt;
					rc = sqlite3_prepare_v2(db, delete_sql.c_str(), -1, &delete_stmt, nullptr);
					if (rc != SQLITE_OK) {
						std::cerr << "Delete SQL error: " << sqlite3_errmsg(db) << std::endl;
						sqlite3_finalize(stmt);
						error="Database error";
					}

					// 执行删除语句
					if (sqlite3_step(delete_stmt) != SQLITE_DONE) {
						std::cerr << "Delete error: " << sqlite3_errmsg(db) << std::endl;
						sqlite3_finalize(stmt);
						sqlite3_finalize(delete_stmt);
						error="Database error";
					}

					// 完成删除后，需要重置和销毁delete_stmt
					sqlite3_finalize(delete_stmt);

					if(error.size()) {				
						return crow::response(500, error);
					}
				}
			}
			if (req_type=="student") {
				// 查询的sql语句，查找到req_id这条记录
				string sql = "SELECT * FROM requests_student WHERE req_id = ?;";

				sqlite3_stmt* stmt;
				string error;
				// 预处理sql语句
				int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
				if (rc != SQLITE_OK) {
					cerr << "SQL error" << endl;
					error="Database error";
				}
		
				// 将sql语句中的占位符(?)链接到变量
				sqlite3_bind_text(stmt, 1, req_id.c_str(), -1, SQLITE_STATIC);
				
				//查找到记录后，将这条记录删除
				if (sqlite3_step(stmt) == SQLITE_ROW) {
					// 找到了记录，现在执行删除操作
					std::string delete_sql = "DELETE FROM requests_student WHERE req_id = ?;";

					sqlite3_stmt* delete_stmt;
					rc = sqlite3_prepare_v2(db, delete_sql.c_str(), -1, &delete_stmt, nullptr);
					if (rc != SQLITE_OK) {
						std::cerr << "Delete SQL error: " << sqlite3_errmsg(db) << std::endl;
						sqlite3_finalize(stmt);
						error="Database error";
					}

					// 将删除语句中的占位符(?)绑定到变量req_id
					sqlite3_bind_text(delete_stmt, 1, req_id.c_str(), -1, SQLITE_STATIC);

					// 执行删除语句
					if (sqlite3_step(delete_stmt) != SQLITE_DONE) {
						std::cerr << "Delete error: " << sqlite3_errmsg(db) << std::endl;
						sqlite3_finalize(stmt);
						sqlite3_finalize(delete_stmt);
						error="Database error";
					}

					// 完成删除后，需要重置和销毁delete_stmt
					sqlite3_finalize(delete_stmt);

					if(error.size()) {				
						return crow::response(500, error);
					}
				}
			}        
		} 

		return crow::response(200, "Default");
	});

	CROW_ROUTE(app, "/info_modify").methods("POST"_method)([db](const crow::request& req) {
		auto body = crow::json::load(req.body); // 获取请求体中的 JSON

		if (!body) {
			return crow::response(400, "Invalid request body");
		}

		// 从请求体中获取学生的ID和要修改的字段
		string req_id = body["req_id"].s();
		int id = body["id"].i();
		string name = body["name"].s();
		int gender = body["gender"].i();
		string phone_number = body["phone_number"].s();
		string wish = body["wish"].s();

		// 插入到 pending_changes 表中，等待管理员审核
		string sql = "INSERT INTO requests_student (req_id, id, name, gender, phone_number, wish) VALUES (?, ?, ?, ?, ?, ?);";
		sqlite3_stmt* stmt;

		int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
		if (rc != SQLITE_OK) {
			cerr << sqlite3_errmsg(db) << endl;
			return crow::response(500, "Database error");
		}
		
		sqlite3_bind_text(stmt, 1, req_id.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 2, id);
		sqlite3_bind_text(stmt, 3, name.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 4, gender);
		sqlite3_bind_text(stmt, 5, phone_number.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 6, wish.c_str(), -1, SQLITE_STATIC);

		rc = sqlite3_step(stmt);
		if (rc != SQLITE_DONE) {
			sqlite3_finalize(stmt);
			return crow::response(500, "Failed to insert pending change");
		}

		sqlite3_finalize(stmt);

		return crow::response(200, "Your request has been submitted for review");
	});

	app.bindaddr("0.0.0.0").port(18080).multithreaded().run();

	sqlite3_close(db);
}