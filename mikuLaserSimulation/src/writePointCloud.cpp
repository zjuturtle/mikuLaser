
//void writePCDascii(const std::string &file_name, const std::vector<Vector3>& pcd, const int precision = 8)
//{
//
//	if (pcd.empty())
//	{
//		WARN_THROTTLE(1, "Got empty point cloud");
//		return;
//	}
//
//	if (isnan(pcd[0].x()))
//	{
//		WARN_THROTTLE(1, "Got NaN");
//		return;
//	}
//
//	std::ofstream fs;
//	fs.open(file_name.c_str());      // Open file
//
//	if (!fs.is_open() || fs.fail())
//	{
//		WARN_THROTTLE(1, "Could not open file for writing!");
//		return;
//	}
//
//	auto pointCount = pcd.size();
//	std::cout << "writePCDascii: pointCount " << pointCount << std::endl;
//
//	// Mandatory lock file
//	// boost::interprocess::file_lock file_lock;
//	// setLockingPermissions (file_name, file_lock);
//
//	fs.precision(precision);
//	fs.imbue(std::locale::classic());
//	// Write the header information
//	fs << "# .PCD v0.7 - Point Cloud Data file format"
//		<< "\nVERSION 0.7"
//		<< "\nFIELDS x y z" //"intensity"
//		<< "\nSIZE 4 4 4"
//		<< "\nTYPE F F F"
//		<< "\nCOUNT 1 1 1"
//		<< "\nWIDTH " << pointCount
//		<< "\nHEIGHT " << 1
//		<< "\nPOINTS " << pointCount
//		<< "\nDATA ascii\n";
//	std::ostringstream stream;
//	stream.precision(precision);
//	stream.imbue(std::locale::classic());
//	// Iterate through the points
//	for (auto i = 0u; i < pointCount; ++i) {
//		const auto& point = pcd[i];
//		//        fs << point.x << " "  << point.y << " "  << point.z << " "  << pc_msg_ptr->channels[0].values[i] << std::endl;
//		stream << boost::numeric_cast<float>(point.x())
//			<< " " << boost::numeric_cast<float>(point.y())
//			<< " " << boost::numeric_cast<float>(point.z());
//		//            << " " << boost::numeric_cast<float>(pc_msg_ptr->channels[0].values[i]);
//		// Copy the stream, trim it, and write it to disk
//		std::string result = stream.str();
//		boost::trim(result);
//		stream.str("");
//		fs << result << "\n";
//		//        usleep(1000);
//	}
//	fs.close();              // Close file
//							 // resetLockingPermissions (file_name, file_lock);
//
//	std::cout << "pcd file generated." << std::endl;
//}