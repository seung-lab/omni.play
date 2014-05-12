#pragma once

#define GL_GLEXT_PROTOTYPES

#if defined(__APPLE_CC__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#endif

#include <algorithm>
#include <array>
#include <assert.h>
#include <atomic>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/date_time.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/functional/hash.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <boost/log/common.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions/attr.hpp>
#include <boost/log/expressions/filter.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/expressions/predicates/is_in_range.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/utility/empty_deleter.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/multi_array.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/optional.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/regex.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/variant.hpp>
#include <cassert>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <curl/curl.h>
#include <deque>
#include <exception>
#include <float.h>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QColor>
#include <QCoreApplication>
#include <QDataStream>
#include <QDateTime>
#include <QDialog>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileInfoList>
#include <QFont>
#include <QFrame>
#include <QGLContext>
#include <QGLPixelBuffer>
#include <QGLWidget>
#include <QGridLayout>
#include <QGroupBox>
#include <QHash>
#include <QImage>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QListWidget>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QSet>
#include <QShortcut>
#include <QSize>
#include <QSlider>
#include <QString>
#include <QStringList>
#include <QtCore/QVariant>
#include <QTextStream>
#include <QtGlobal>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>
#include <QtGui>
#include <QThread>
#include <QTimer>
#include <QToolBox>
#include <QUndoCommand>
#include <QUndoStack>
#include <QUndoView>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>
#include <set>
#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <strnatcmp.h>
#include <thread>
#include <tuple>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <vmmlib/vmmlib.h>
#include <yaml-cpp-old/yaml.h>
#include <zi/array.hpp>
#include <zi/assert.hpp>
#include <zi/atomic/atomic.hpp>
#include <zi/bits/cstdint.hpp>
#include <zi/bits/enable_shared_from_this.hpp>
#include <zi/bits/hash.hpp>
#include <zi/bits/shared_ptr.hpp>
#include <zi/bits/type_traits.hpp>
#include <zi/concurrency.hpp>
#include <zi/concurrency/condition_variable.hpp>
#include <zi/concurrency/config.hpp>
#include <zi/concurrency/guard.hpp>
#include <zi/concurrency/mutex.hpp>
#include <zi/concurrency/runnable.hpp>
#include <zi/concurrency/rwmutex.hpp>
#include <zi/concurrency/semaphore.hpp>
#include <zi/concurrency/spinlock.hpp>
#include <zi/concurrency/thread.hpp>
#include <zi/for_each.hpp>
#include <zi/mesh/marching_cubes.hpp>
#include <zi/mesh/quadratic_simplifier.hpp>
#include <zi/mesh/tri_mesh.hpp>
#include <zi/mesh/tri_stripper.hpp>
#include <zi/meta/enable_if.hpp>
#include <zi/mutex.hpp>
#include <zi/parallel/algorithm.hpp>
#include <zi/parallel/numeric.hpp>
#include <zi/singleton.hpp>
#include <zi/system.hpp>
#include <zi/time.hpp>
#include <zi/unordered_map.hpp>
#include <zi/utility/assert.hpp>
#include <zi/utility/container_utilities.hpp>
#include <zi/utility/enable_singleton_of_this.hpp>
#include <zi/utility/non_copyable.hpp>
#include <zi/vl/vec.hpp>
#include <zi/zargs/zargs.hpp>